#include "block.h"
#include "utils.h"
#include "crc.h"

/**
 * @brief 构造
 * 
 * @param max_group_size 初始化一个 group 内的键值对容量
 */
BlockBuilder::BlockBuilder(int max_group_size) : max_group_size_(max_group_size) {}

/**
 * @brief 清空键值对
 * 
 * 清空键值对组、所有重启点、编码内容
 */
void BlockBuilder::clear() {
    least_entries_.clear();
    restarts_.clear();
    content_.clear();
}

/**
 * @brief 获取内部键值对的数量
 * 
 * @return size_t 返回的数量
 * 
 * 调用内部键值对组 least_entries 的函数即可
 */
size_t BlockBuilder::size() {
    return least_entries_.size();
}

/**
 * @brief 判断是否为空（没有键值对）
 * 
 * @return bool 是否为空
 *     @retval true 空
 *     @retval false 非空
 * 
 * 调用内部键值对组 least_entries 的函数即可
 */
bool BlockBuilder::empty() {
    return least_entries_.empty();
}

/**
 * @brief 将 kv 传入 least_entries
 * 
 * @param key 传入键
 * @param value 传入值
 * 
 * 直接 push 进键值对组即可
 */
void BlockBuilder::addEntry(const std::string& key, const std::string& value) {
    least_entries_.emplace_back(key, value);
}

/**
 * @brief 排序 entry，写好 restart[], restart_nums, type, crc
 * 
 * finish() 直接在 conent_ 后面按键值对组添加一整个编码后的 block
 */
void BlockBuilder::finish() {
    // entry[] 排序后生成 restarts[]，并将 entry 扩充进入 content_
    std::string last_key;
    std::sort(least_entries_.begin(), least_entries_.end());
    for (int i = 0; i < least_entries_.size(); i ++) {
        if (i % max_group_size_ == 0) {
            last_key.clear();
            restarts_.push_back(content_.size());
        }
        content_.append(least_entries_[i].encode(last_key));
        last_key = least_entries_[i].key();
    }
    // 编码 restarts[] 
    for (int i = 0; i < restarts_.size(); i ++) {
        setFixed32(&content_, restarts_[i]);       
    }
    // 生成 restart_number
    setFixed32(&content_, restarts_.size());
    // type: 1=压缩后
    content_.append(type_to_string(char(1)));
    // 生成 crc
    setFixed32(&content_, crc32((uint8_t*)content_.data(), content_.size()));
}

/**
 * @brief 提交缓冲区(content)给 dst
 * 
 * @param dst 目标刷入字符串
 * 
 * 此行为在 finish 之前没有意义。
 */
void BlockBuilder::flush(std::string* dst) {
    dst->append(content_);
}

// -------------------------------------------------- Block

/**
 * @brief 前置解析：将 restart_number 和 restart_offset 初始化完成
 * 
 * 提取 content_ 内后九个字节。
 * 前 4 个是 restart_number。
 * 利用 restart_number 可以获取到 restart 首位在哪，也就是 restart_offset。
 */
void Block::preParse() {
    size_t start_offset = content_.size() - 9;
    restart_number = getFixed32(content_.substr(start_offset, 4));
    restart_offset = content_.size() - 9 - 4 * restart_number;
}

/**
 * @brief Construct a new Block:: Block object
 * 
 * @param max_group_size 一组内键值对最大容量
 * 
 * 空构造，变量都是零值
 */
Block::Block(int max_group_size) : max_group_size_(max_group_size), content_(""), restart_number(0), restart_offset(0) {}

/**
 * @brief Construct a new Block:: Block object
 * 
 * @param content 已有的 block 编码串
 * @param max_group_size 一组内键值对最大容量
 * 
 * 利用已有的编码解析出 block
 */
Block::Block(const std::string& content, int max_group_size) : max_group_size_(max_group_size), content_(content) {
    preParse();
}
Block::Block(const char* s, int max_group_size) : max_group_size_(max_group_size), content_(s) {
    preParse();
}
Block::Block(const Block& that) :
        content_(that.content_),
        restart_number(that.restart_number),
        restart_offset(that.restart_offset),
        max_group_size_(that.max_group_size_) 
{}

/**
 * @brief 解析出第 idx 个键值对
 * 
 * @param idx 解析键值对的编号
 * @return Entry 解析出来的键值对
 * 
 * 通过 idx 获取位于第几个 group 内，然后通过 group(group_id) 和 group(group_id + 1) 或 restart(0) 切出 group 的编码。
 * 在这个 group 内从首 key(完整key) 开始往后走到第 idx % max_group_size_ 个也就是要解析的键值对，
 * 即可把该键值对的完整 key 和 value 推出来。
 */
Entry Block::decodeEntry (int idx) {
    // TODO: 加上 cache 缓存几个 group ，就可以省去长度最长为 16 的遍历
    /* 第 idx 个键值对位于第 group_id 个组内 */
    int group_id = idx / max_group_size_;
    size_t group_offset = getFixed32(content_.substr(restart_offset + 4 * group_id, 4));
    size_t next_group_offset;
    if (group_id + 1 == restart_number) { // 是最后一个组，结尾编码是 restart(0) 的位置
        next_group_offset = restart_offset;
    } else {  // 不是最后一个组，结尾编码是下一个组的首地址
        next_group_offset = getFixed32(content_.substr(restart_offset + 4 * (group_id + 1), 4));
    }
    /* 第 group_id 个组的编码 */
    Slice group = content_.substr(group_offset, next_group_offset - group_offset);
    std::string key;
    std::string value;
    for (int i = 0; i <= idx % max_group_size_; i ++) {
        Entry entry;
        entry.decode(group, key);
        if (i == idx % max_group_size_) {
            value = entry.value();
        }
    }
    return Entry(key, value);
}

/**
 * @brief 解析出第 group_id 个组的所有键值对
 * 
 * @param group_id 待解析组号
 * @return std::vector<Entry> 解析出来的键值对们
 * 
 * 与 decodeEntry(idx) 省去了求 group_id 的过程，别的都一样，
 * 在最后从首键值对往后推的时候每一个都要塞入返回中。 
 */
std::vector<Entry> Block::decodeEntries (int group_id) {
    if (group_id >= restart_number)
        return {};
    // TODO: 加上 cache 缓存几个 group ，就可以省去长度最长为 16 的遍历
    size_t group_offset = getFixed32(content_.substr(restart_offset + 4 * group_id, 4));
    size_t next_group_offset;
    if (group_id + 1 == restart_number) {
        next_group_offset = restart_offset;
    } else {
        next_group_offset = getFixed32(content_.substr(restart_offset + 4 * (group_id + 1), 4));
    }

    std::vector<Entry> ret;
    Slice group = content_.substr(group_offset, next_group_offset - group_offset);
    std::string key;
    std::string value;
    for (int i = 0; i < max_group_size_ && group.size(); i ++) {
        Entry entry;
        entry.decode(group, key);
        ret.push_back(entry);
    }
    return ret;
}

/**
 * @brief 将所有的键值对都解析出来
 * 
 * @return std::vector<Entry> 解析出来的 entries
 */
std::vector<Entry> Block::decodeEntries() {
    std::vector<Entry> ret;
    for (int i = 0; ; i ++) {
        bool has_entry = false;
        for (auto it : decodeEntries(i)) {
            ret.push_back(it);
            has_entry = true;
        }
        if (!has_entry)
            break;
    }
    return ret;
}

/**
 * @brief 获取 key 对应的 value
 * 
 * @param key 目标键
 * @return std::string 返回查询的 value 结果
 *     @retval "" 空字符串代表没有找到
 *     @retval "..." 非空字符串代表找到了
 * 
 * 找每一个组的首键，通过二分找首键<=key的最后一个组，
 * key 要么在这个组里面，要么不在这个块里面，扫描一下便可判断出来。
 */
std::string Block::getValue(const std::string &key) {
    int group_id = 0;
    /* 最后一个组首键不大于，就定位到最后一个即可
       否则二分出最后一个首键不大于key的组号 */
    if (decodeEntry((restart_number - 1) * max_group_size_).key() <= key) {
        group_id = restart_number - 1;
    } else {
        int l = 0, r = restart_number - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (decodeEntry(mid * max_group_size_).key() <= key)
                group_id = mid,
                l = mid + 1;
            else
                r = mid - 1;
        }
    }
    /* 扫描全组判断 */
    for (Entry block_entry : decodeEntries(group_id)) {
        if (key == block_entry.key()) 
            return block_entry.value();
        if (key < block_entry.key()) 
            break;
    }
    return "";
}
