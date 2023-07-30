#include "table.h"
#include "utils.h"
#include "option.h"
#include <fstream>
#include <cstring>
#include <algorithm>

/**
 * @brief 限制 data_block[i] 的键值对数量，从零开始塞入编码器
 * 
 * @param one_block_size 一个 datablock 的键值对容量
 */
TableBuilder::TableBuilder(size_t one_block_size) : 
        one_block_size_(one_block_size),
        index_block_(BlockBuilder(1)) 
{}

/**
 * @brief 将键值对塞入 least_entries ，先不开始化编码
 * 
 * @param key 键
 * @param value 值
 */
void TableBuilder::addEntry(const std::string& key, const std::string& value) {
    least_entries.push_back(Entry(key, value));
}

/**
 * @brief 将所有的 {key,value} 生成 block 和 footer
 * 
 * 排序后按顺序构造 data_block[...] ，划分完后将每一个 datablock[i] 编码并刷入 content_。
 * 另一边同时记录 indexblock 的信息，datablock[...] 刷入完立即编码 indexblock。
 * 最后编码 footer。
 * 
 */
void TableBuilder::finish() {
    std::sort(least_entries.begin(), least_entries.end());
    for (Entry& entry : least_entries) {
        if (data_block_.empty() || data_block_.back().size() == one_block_size_) {
            data_block_.push_back(BlockBuilder(16));
        }
        data_block_.back().addEntry(entry.key(), entry.value());
        if (data_block_.back().size() == one_block_size_) {
            data_block_.back().finish();
        }
    }
    if (data_block_.back().size() != one_block_size_) {
        data_block_.back().finish();
    }

    // 创建 index_block ，并将 data_block[] 刷到 content_
    content_.clear();
    for (int i = 0; i < data_block_.size(); i ++) {
        // value
        int32_t offset = content_.size();
        data_block_[i].flush(&content_);
        int32_t size = content_.size() - offset;
        std::string value;
        setFixed32(&value, offset);
        setFixed32(&value, size);
        // key：根据 key1，key2，生成大于 key1 小于 key2 的最短的
        std::string key;
        if (i + 1 < data_block_.size()) {
            std::string key1 = data_block_[i].least_entries_.back().key();
            std::string key2 = data_block_[i + 1].least_entries_[0].key();
            for (int j = 0; j < std::min(key1.size(), key2.size()); j ++) {  
                if (key1[j] != key2[j]) {
                    if (key1[j] + 1 == key2[j] && j + 1 == key2.size()) {
                        key += key1[j];
                        if (j + 1 == key1.size()) {
                            key += char(0);
                        } else {
                            key += key1[j + 1] + 1;
                        }
                    } else {
                        key += key1[j] + 1;
                    }
                    break;
                } else {
                    key += key1[j];
                }
                if (j + 1 == key1.size()) {
                    key += char(0);
                }
            }
        } else {
            std::string key1 = data_block_[i].least_entries_.back().key();
            for (int j = 0; j < key1.size(); j ++) {
                if (key1[j] != 'z') {
                    key += key1[j] + 1;
                    break;
                } else {
                    key += 'z';
                }
            }
        }
        index_block_.addEntry(key, value);
    }
    // 将 index_block 刷到 content_
    index_block_.finish();
    size_t index_block_offset = content_.size();
    index_block_.flush(&content_);
    // 建立 footer，并刷到 content_
    size_t index_block_size = content_.size() - index_block_offset;
    footer_.addIndexBlock(index_block_offset, index_block_size);
    footer_.finish(Option::magic_number);
    footer_.flush(&content_);
}

/**
 * @brief 将字节码写入文件 "${file_name}" 中
 * 
 * @param file_name 待写入文件
 */
void TableBuilder::flush(const std::string& file_name) const {
    std::ofstream fout(file_name);
    fout << content_;
    fout.flush();
    fout.close();
}

/**
 * @brief 获取键值对数量
 * 
 * @return size_t 键值对数量
 */
size_t TableBuilder::size () {
    return least_entries.size();
}

/**
 * @brief 获取编码内容
 * 
 * @return std::string 当前已编码的内容
 */
std::string TableBuilder::content() {
    return content_;
}

// ------------------------ Table

/**
 * @brief 构造，读取文件 file_path 先解析出 footer_ 和 indexblock
 * 
 * @param file_path 待读取文件路径
 */
Table::Table(const std::string& file_path) : fin_(std::make_shared<std::ifstream>(file_path)), index_block_(Block(1)), file_path_(file_path) {
    char* footer_s = new char[24];
    fin_->seekg(-24, std::ios::end);
    fin_->read(footer_s, 24);
    footer_ = Footer(std::string(footer_s, 24));
    auto [index_offset, index_size] = footer_.getBlock(0);
    fin_->seekg(index_offset);
    char* index_block_s = new char[index_size + 2];
    bzero(index_block_s, sizeof(index_block_s));
    fin_->read(index_block_s, index_size);
    index_block_ = Block(std::string(index_block_s, index_size), 1);
    delete index_block_s;
    delete footer_s;
}

/**
 * @brief 强制深拷贝构造
 * 
 * @param that 构造目标
 */
Table::Table (const Table& that) :
        file_path_(that.file_path_),
        fin_(that.fin_),
        footer_(that.footer_),
        index_block_(that.index_block_)
{}


/**
 * @brief 析构，关闭文件读取器
 */
Table::~Table() {
}

/**
 * @brief 重构造，读取文件 file_path 先解析出 footer_ 和 indexblock
 * 
 * @param file_path 待读取文件路径
 */
void Table::reload(const std::string& file_path) {
    file_path_ = file_path;
    fin_->close();
    fin_.reset(new std::ifstream(file_path));
    char* footer_s = new char[24];
    fin_->seekg(-24, std::ios::end);
    fin_->read(footer_s, 24);
    footer_ = Footer(std::string(footer_s, 24));
    auto [index_offset, index_size] = footer_.getBlock(0);
    fin_->seekg(index_offset);
    char* index_block_s = new char[index_size + 2];
    bzero(index_block_s, sizeof(index_block_s));
    fin_->read(index_block_s, index_size);
    index_block_ = Block(std::string(index_block_s, index_size), 1);
    delete index_block_s;
    delete footer_s;
}

/**
 * @brief 获取编号为 block_id 的 block 编码内容
 * 
 * @param block_id 块号
 * @return std::string 该块的全部编码
 */
std::string Table::getBlock (int block_id) {
    if (block_id >= index_block_.restart_number)
        return "";
    Slice data_block_info = index_block_.decodeEntry(block_id).value();
    uint64_t data_block_offset = getFixed32(data_block_info);
    uint64_t data_block_size = getFixed32(data_block_info);
    fin_->seekg(data_block_offset);
    char* data_block_s = new char[data_block_size + 2];
    bzero(data_block_s, sizeof(data_block_s));
    fin_->read(data_block_s, data_block_size);
    std::string ret(data_block_s, data_block_size);
    delete data_block_s;
    return ret;
}

/**
 * @brief 根据 key 获得 value
 * 
 * @param key 目标键
 * @return std::string 要获取的值
 *     @retval "" key不存在
 *     @retval else 成功获取到的值
 * 
 * 先通过 index_block 的 key 二分出在哪个 data_block 内，
 * 返回的是在这个 data_block[i] 中调用 getValue 的结果。
 */
std::string Table::getValue (const std::string& key) {
    int block_id = 0;
    {
        int l = 0, r = index_block_.restart_number - 1;
        while (l <= r) {
            int mid = (l + r) >> 1;
            if (index_block_.decodeEntry(mid).key() > key)
                r = mid - 1,
                block_id = mid;
            else
                l = mid + 1;
        }
    }
    return Block(getBlock(block_id), 16).getValue(key);
}

