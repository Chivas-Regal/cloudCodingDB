#ifndef _INCLUDE_DB_DB_IMPL_H_
#define _INCLUDE_DB_DB_IMPL_H_

#include "lru.h"
#include "table.h"
#include "skiplist.h"
#include "utils.h"
#include "manifest.h"
#include "wal.h"
#include "mempool.h"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <shared_mutex>
#include <memory>

class ManiFest;

class DBImpl {
public:
    /* 参数在下面均有成员变量 */
    DBImpl (size_t cache_size, size_t sst_size, size_t sst_one_block_size, const std::string& username);

    /* 除了这三个有成员变量的参数，还有一个隐式赋值变量 sst_one_block_size 为 sqrt(sst_size) */
    DBImpl (size_t cache_size, size_t sst_size, const std::string& username);
    
    /* 若数据库未 close 就进行一次 close */
    ~DBImpl ();
public:
    /* 填入键值对，对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false */
    void put (const std::string& key, std::pair<uint8_t*, size_t> value, bool new_kv = true);

    /* 填入键值对，对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false */
    void put (const std::string& key, const std::string& value, bool new_kv = true);

    /* 利用 key 访问 value */
    std::pair<uint8_t*, size_t> get (const std::string& key);

    /* 删除键 key */
    void erase (const std::string& key);

    /* 调用 TableBuilder 将 mem_table 内的数据刷到磁盘上并顺序存储文件名 */
    void flush ();

    /* 关闭，并将 lrucache 内的所有内容放到 memtable 并刷盘 */
    void close ();
private:
    size_t cache_size_;         ///< 缓存中的最大数量
    size_t sst_size_;           ///< 一个 sst 文件中的最大键值对数量
    size_t sst_one_block_size_; ///< 一个 sst.block 中最大键值对数量
    std::string username_;      ///< 登录用户名，可以定位 sst 文件位置
    std::string folderpath_;    ///< 此用户 sst 文件夹位置
    std::vector<std::string> sstfiles_; ///< 所有 sst 文件路径
    bool has_closed;            ///< 数据库是否 close 过
    std::shared_ptr<ManiFest> manifest; ///< lsmtree 管理器
    std::shared_ptr<MemPool> mempool; ///< 内存池
private:
    WAL_MEMTABLE wal_memtable;   ///< 保障 wal_memtable
    LRUCache<std::pair<uint8_t*, size_t>> lru_;  ///< cache
    KV_SkipList<std::string> mem_table_; ///< 未刷盘的 sst
private: // LOCK
    std::shared_mutex mutex;
};


#endif