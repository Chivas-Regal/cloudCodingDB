#ifndef _INCLUDE_SST_TABLE_
#define _INCLUDE_SST_TABLE_

#include <vector>
#include <fstream>
#include <memory>
#include "block.h"
#include "footer.h"

/**
 * @brief sst 编码器
 * 
 *      +-------------------+
 *      |    data block 1   |
 *      +-------------------+
 *      |    data block 2   |
 *      +-------------------+
 *               ...
 *      +-------------------+
 *      |    data block N   |
 *      +-------------------+
 * TODO |    filter block   |
 * TODO +-------------------+
 * TODO |  metaindex block  |
 * TODO +-------------------+
 *      |    index block    |
 *      +-------------------+
 *      |      footer       |
 *      +-------------------+
 * 
 * 在一步步 addEntry() 构成的过程中，先仅仅将其添加到 least_entries 中。
 * 当进行 finish 的时候，先排序，然后生成所有 data_block_[...] 部分的编码并刷到 content_ 中。
 * 接着在刷 data_block_[...] 的时候，每有一个 data_block_[i] 的信息就生成一个 index_block[i] 的信息。
 * 等 data_block_[...] 刷完后刷入 index_block_，再把 footer 刷入。
 */
class TableBuilder {
public:
    /* 限制 data_block[i] 的键值对数量，从零开始塞入编码器 */
    TableBuilder (size_t one_block_size);

    ~TableBuilder () = default;

    /* 将 {key,value} 塞入 least_entries ，先不开始化编码 */
    void addEntry (const std::string& key, const std::string& value);

    /* 将所有的 {key,value} 生成 block 和 footer */
    void finish ();

    /* 将字节码写入文件 "${file_name}" 中 */
    void flush (const std::string& file_name) const;
    
    /* 获取键值对数量 */
    size_t size ();

    /* 获取编码内容 */
    std::string content ();
private:
    std::vector<Entry> least_entries;       ///< 所有的键值对
    
    size_t one_block_size_;                 ///< 一个 data_block_ 的键值对容量
    std::vector<BlockBuilder> data_block_;  ///< 所有 data_block_
    BlockBuilder index_block_;              ///< index_block KV(最短的超过maxkey的串：offset_size)
    FooterBuilder footer_;                  ///< 尾部

    std::string content_;
friend class Compactor;
friend class ManiFest;
};

/**
 * @brief sst 解码工具
 * 
 * 注：这里所有的读取都是以 block 为单位：
 *     - 不会将文件中过多的内容一次全读到内存导致崩溃。
 *     - 不会一次读取太少要读取太多次影响效率。
 * 
 * 首先将配置信息 footer_ 提出来。
 * 既然每次读取一个 block 是足够的，这里把索引块先解析出来存放着也可以。
 */
class Table {
public:
    /* 给定 sst 文件路径 */
    Table (const std::string& file_path);

    /* 深拷贝构造 */
    Table (const Table& that);

    /* 析构，关闭文件读取器 */
    ~Table ();

    /* 重载，打开 file_path */
    void reload (const std::string& file_path);

    /* 根据 block_id 获取 block 全部编码内容 */
    std::string getBlock (int block_id);

    /* 根据 key 获得 value */
    std::string getValue (const std::string& key);
private:
    std::string file_path_; ///< sst 文件路径
    std::shared_ptr<std::ifstream> fin_; ///< 用来读取 sst 文件的读取流
    Footer footer_;     ///< 解析出来的一个文件的尾部
    Block index_block_; ///< 解析出来的一个文件的索引块
};

#endif