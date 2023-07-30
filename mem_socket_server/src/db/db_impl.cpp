#include "db_impl.h"
#include "proj_info.h"
#include "config.h"
#include "utils.h"
#include "utils/limits.h"
#include <cmath>
#include <filesystem>

/**
 * @brief 构造
 * 
 * @param cache_size 缓存器的键值对容量
 * @param sst_size 一个 sst 文件的键值对容量
 * @param sst_one_block_size sst 内一个 datablock 的键值对容量
 * @param username 用户名
 * 
 * 设置操作文件夹为 ${PROJECT_PATH}/userdata/<username>/。
 */
DBImpl::DBImpl(size_t cache_size, size_t sst_size, size_t sst_one_block_size, const std::string& username) :
        cache_size_(cache_size),
        sst_size_(sst_size),
        sst_one_block_size_(sst_one_block_size),
        lru_(LRUCache<std::pair<uint8_t*, size_t>>(cache_size)),
        username_(username),
        folderpath_(std::string(PROJECT_PATH) + "/userdata/" + username + "/"),
        has_closed(false),
        manifest(std::make_shared<ManiFest>(username)),
        wal_memtable(folderpath_ + "WAL_memtable.log"),
        mempool(std::make_shared<MemPool>(MEM_NUM_LISTS, MEM_SIZE_LIST, MEM_POOL_ALGO))
{
    if (wal_memtable.content_.size()) {
        for (auto it : wal_memtable.getElements()) {
            mem_table_.put(it.key(), it.value());
        }
    }
}

/**
 * @brief 构造
 * 
 * @param cache_size 缓存器的键值对容量
 * @param sst_size 一个 sst 文件的键值对容量
 * @param username 用户名
 * 
 * sst_one_block_size 默认为 sqrt(sst_size) ，分块平分外层查询和内层查询的复杂度。
 * 设置操作文件夹为 ${PROJECT_PATH}/userdata/<username>/。
 */
DBImpl::DBImpl(size_t cache_size, size_t sst_size, const std::string& username) :
        cache_size_(cache_size),
        sst_size_(sst_size),
        sst_one_block_size_(std::sqrt(sst_size)),
        lru_(LRUCache<std::pair<uint8_t*, size_t>>(cache_size)),
        username_(username),
        folderpath_(std::string(PROJECT_PATH) + "/userdata/" + username + "/"),
        has_closed(false),
        manifest(std::make_shared<ManiFest>(username)),
        wal_memtable(folderpath_ + "WAL_memtable.log"),
        mempool(std::make_shared<MemPool>(MEM_NUM_LISTS, MEM_SIZE_LIST, MEM_POOL_ALGO))
{
    if (wal_memtable.content_.size()) {
        for (auto it : wal_memtable.getElements()) {
            mem_table_.put(it.key(), it.value());
        }
    }
}

/**
 * @brief 析构
 * 
 * 要至少将数据库关闭（归档lrucache的所有键值对）。
 */
DBImpl::~DBImpl() {
    if (!has_closed) {
        close();
    }
}

/**
 * @brief 填入键值对
 * 
 * @param key 键
 * @param value 值
 * @param new_kv 对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false
 * 
 * put() 算写操作，上互斥锁。
 * 若 lru 这次填入有需要刷到磁盘上的弹出键值对，应填入 mem_table。
 * 若 mem_table 已满，就刷到磁盘上然后再接收 {key,value}。
 */
void DBImpl::put (const std::string& key, std::pair<uint8_t*, size_t> value, bool new_kv) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto v_before = lru_.get(key);
    if (v_before.second)
        mempool->deallocate(v_before.first, v_before.second);
    auto [k, v] = lru_.put(key, value, new_kv);
    // 要么是没有弹出，要么是非修改键
    if (k != "") {
        if (mem_table_.size() == sst_size_ && !mem_table_.count(k)) {
            flush();
            mem_table_.clear();
        }
        std::string value_s = std::string((char*)v.first, v.second);
        mempool->deallocate(v.first, v.second);
        mem_table_.put(k, value_s);
        wal_memtable.putElement(k, value_s);
    } else if (v.second) {
        // 非修改，但是确实有弹出
        mempool->deallocate(v.first, v.second);
    }
}

/**
 * @brief 填入键值对
 * 
 * @param key 键
 * @param value 值
 * @param new_kv 对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false
 * 
 * 字符串格式的 value ，直接调用指针形式的 put 即可
 */
void DBImpl::put (const std::string& key, const std::string& value, bool new_kv) {
    std::pair<uint8_t*, size_t> ret = {(uint8_t*)mempool->allocate(value.size()), value.size()};
    std::copy(value.begin(), value.end(), ret.first);
    put(key, ret, new_kv);
}

/**
 * @brief 通过 key 获取 value
 * 
 * @param key 目标 key
 * @return std::string 获取到的 value
 *     @retval "not found" 查无此 key
 *     @retval else        获取到了 value
 * 
 * 开启共享锁，然后按查询缓存、内存表、主存表的顺序查询 key 直到查询成功为止。
 * 都没有 return 的话就是没找到，返回 not found。
 */
std::pair<uint8_t*, size_t> DBImpl::get (const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::pair<uint8_t*, size_t> value = lru_.get(key);
    /* 1.查询缓存 */
    if (value.second != 0) {
        lock.unlock();
        return value;
    }
    /* 2.查询内存表 */
    if (mem_table_.count(key)) {
        std::string get_value = mem_table_.get(key);
        lock.unlock();
        std::pair<uint8_t*, size_t> ret = {(uint8_t*)mempool->allocate(get_value.size()), get_value.size()};
        std::copy(get_value.begin(), get_value.end(), ret.first);
        put(key, ret, false);
        return ret;
    }
    /* 3.查询主存表 */
    for (int i = 0; i < manifest->_levels_filenames.size(); i ++) {
        if (i == 0) {
            for (int j = manifest->insert_file_id == 1 ? 5 : manifest->insert_file_id - 1; j != manifest->insert_file_id; j = j == 1 ? 5 : j - 1) {
                std::string sst_filepath = std::string(PROJECT_PATH) + "/userdata/" + username_ + "/0/data" + std::to_string(j) + ".sst";
                Table table(sst_filepath);
                std::string get_value = table.getValue(key);
                if (!get_value.empty()) {
                    lock.unlock();
                    std::pair<uint8_t*, size_t> ret = {(uint8_t*)mempool->allocate(get_value.size()), get_value.size()};
                    std::copy(get_value.begin(), get_value.end(), ret.first);
                    put(key, ret, false);
                    if (get_value != DEL_STRING) {
                        return ret;
                    } else 
                        return {nullptr, 0};
                }
            }
        } else {
            _manifest_record target;
            target._min_key = key;
            target._max_key = MAX_STRING;
            auto lower_manifestrecord = manifest->_levels_files[i].lower(target)->value;
            if (lower_manifestrecord._min_key.empty())
                continue;
            if (lower_manifestrecord._min_key <= key && key <= lower_manifestrecord._max_key) {
                std::string sst_filepath = std::string(PROJECT_PATH) + "/userdata/" + username_ + "/" + std::to_string(lower_manifestrecord._level_id) + "/" + lower_manifestrecord._file_name;
                Table table(sst_filepath);
                std::string get_value = table.getValue(key);
                if (!get_value.empty()) {
                    lock.unlock();
                    std::pair<uint8_t*, size_t> ret = {(uint8_t*)mempool->allocate(get_value.size()), get_value.size()};
                    std::copy(get_value.begin(), get_value.end(), ret.first);
                    put(key, ret, false);
                    if (get_value != DEL_STRING) {
                        return ret;
                    } else 
                        return {nullptr, 0};
                }
            }
        }
    }
    lock.unlock();
    return {nullptr, 0};
}

/**
 * @brief 删除键 key
 * 
 * @param key 待删除键
 * 
 * @details 不管是否成功，因为删除失败后不会有报错也不会影响什么
 */
void DBImpl::erase(const std::string& key) {
    std::pair<uint8_t*, size_t> del_value = get(key); 
    mempool->deallocate(del_value.first, del_value.second);
    std::string sign_value = DEL_STRING;
    std::pair<uint8_t*, size_t> ret = {(uint8_t*)mempool->allocate(sign_value.size()), sign_value.size()};
    std::copy(sign_value.begin(), sign_value.end(), ret.first);
    put(key, ret);
}

/**
 * @brief 调用 TableBuilder 将 mem_table 内的数据刷到磁盘上并顺序存储文件名
 * 
 * 遍历字典树 getKV() 生成的数组，将所有 kv 都放入 tablebuilder。
 * 将 tablebuilder 信息编码后刷到目标路径文件上。
 */
void DBImpl::flush() {
    if (!mem_table_.size()) 
        return;
    TableBuilder table_builder(sst_one_block_size_);
    for (auto [key, value] : mem_table_.getKV()) {
        table_builder.addEntry(key, value);
    }
    table_builder.finish();
    wal_memtable.clear();
    manifest->put(table_builder);
    mem_table_.clear();
}

/**
 * @brief 关闭，并将 lrucache 内的所有内容放到 memtable 并刷盘
 * 
 * 与 put 基本同构，换成了遍历缓存后直接填进 memtable。
 */
void DBImpl::close() {
    if (has_closed) return;
    has_closed = true;
    std::vector<std::pair<uint8_t*, size_t>> st;
    for (auto [k, v, im] : lru_) {
        st.push_back({v.first, v.second});
        if (!im) {
            mempool->deallocate(v.first, v.second);
            continue;
        }
        if (mem_table_.size() == sst_size_ && !mem_table_.count(k)) {
            flush();
            mem_table_.clear();
        }
        std::string value_s = std::string((char*)v.first, v.second);
        mempool->deallocate(v.first, v.second);
        mem_table_.put(k, value_s);
        wal_memtable.putElement(k, value_s);
    }
    std::sort(st.begin(), st.end());
    flush();
    manifest->flush();
    wal_memtable.clear();
}