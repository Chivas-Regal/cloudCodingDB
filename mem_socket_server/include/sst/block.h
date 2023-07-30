#ifndef _INCLUDE_SST_BLOCK_H_
#define _INCLUDE_SST_BLOCK_H_

#include <vector>
#include "entry.h"

/**
 * @details Block
 * 
 *    +-----------------+ 
 *    |     group 1     | 
 *    +-----------------+
 *    |     group 2     |
 *    +-----------------+
 *           ...
 *    +-----------------+
 *    |     group M     |
 *    +-----------------+
 *    | 4B    restart 1 |
 *    +-----------------+
 *    | 4B    restart 2 |
 *    +-----------------+
 *           ...
 *    +-----------------+
 *    | 4B    restart M |
 *    +-----------------+
 *    | 4B  restart num |
 *    +-----------------+
 *    | 1B         type |
 *    +-----------------+
 *    | 4B          crc |
 *    +-----------------+
 * 
 * @details Group
 * 
 *    +-------------+
 *    |   entry 1   |
 *    +-------------+
 *    |   entry 2   |
 *    +-------------+
 *         ...
 *    +-------------+
 *    |   entry X   |
 *    +-------------+
 * 
 */

class BlockBuilder {
public:
    /* 构造，初始化 block 内一个 group 的最多键值对数量 */
    BlockBuilder (int max_group_size);

    ~BlockBuilder () = default;

    /* 清空存放的所有键值对 */
    void clear ();

    /* 获取内部键值对的数量 */
    size_t size ();

    /* 判断是否为空（没有键值对） */
    bool empty ();

    /* 将 kv 传入 least_entries */
    void addEntry (const std::string& key, const std::string& value);

    /* 排序 entry，写好 restart[], restart_nums, type, crc */
    void finish ();

    /* 提交缓冲区(content)给 dst */
    void flush (std::string* dst);
private:
    std::vector<Entry> least_entries_; ///< 当前 group 里面的 entry 
    std::vector<size_t> restarts_;     ///< restart集合（ group.entry[0] 的块内偏移量）

    size_t max_group_size_;            ///< 一个 group 内最多的 entry 数量
    std::string content_;              ///< 将数据存成字符串 content_（缓冲区） ，为 flush 做准备

friend class TableBuilder;
};

class Block {
public:
    /* 初始化空的block读取器 */
    Block (int max_group_size);

    /* 利用已有字符串初始化block读取器，先解码出关键的块信息变量（restart_number,restart_offset) */
    Block (const std::string& content, int max_group_size);
    Block (const char* s, int max_group_size);
    Block (const Block& that);

    /* 将第 i 个 entry 解析出来 */
    Entry decodeEntry (int idx);

    /* 将第 group_id 个 group 内的 entry 全部解析出来 */
    std::vector<Entry> decodeEntries (int group_id);

    /* 将所有的 entry 都解析出来 */
    std::vector<Entry> decodeEntries ();

    /* 获取 key 对应的 value */
    std::string getValue (const std::string& key);
private:
    /* 前置解析：将 restart_number 和 restart_offset 初始化完成 */
    void preParse ();

    std::string content_;   ///< 一个 block 的 sst 编码状态

    size_t restart_number;  ///< group 也是 restart_points 的数量
    size_t restart_offset;  ///< 第一个 restart_point 的偏移量

    size_t max_group_size_; ///< 一个 group 内最多的 entry 数

friend class Compactor;
friend class Table;
};

#endif