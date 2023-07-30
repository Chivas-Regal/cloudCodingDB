#ifndef _INCLUDE_SST_ENTRY_H_
#define _INCLUDE_SST_ENTRY_H_

#include "slice.h"

/**
 * @brief 一个压缩 k-v 对
 * 
 *   +----------------------+ 
 *   | 1~5B    shared_bytes | 
 *   +----------------------+ 
 *   | 1~5B  unshared_bytes | 
 *   +----------------------+ 
 *   | 1~5B      value_size | 
 *   +----------------------+ 
 *   | <uns-b>B   key_delta | 
 *   +----------------------+ 
 *   | <v-s>B         value | 
 *   +----------------------+ 
 * 
 */
class Entry {
public:
    /* 默认构造， key=value="" */
    Entry ();
    /* 用 key 和 value 构造 */
    Entry (const char* key, const char* value);
    Entry (const std::string& key, const std::string& value);
    Entry (std::string&& key, std::string&& value);
    /* 浅拷贝构造 entry（Slice内的字符串为string，无需深拷贝保护赋值） */
    Entry (const Entry& entry) = default;
    Entry& operator = (const Entry& entry) = default;
    ~Entry() = default;

public:
    /* 获取 key */
    std::string key ();

    /* 获取 value */
    std::string value ();

    /* 传入上一个 entry 的 key 后用自己已有的 {key,value} 生成一个编码后的256进制字符串形式的 entry */
    std::string encode (const std::string& last_key);

    /* 解码 content 前面的内容出一个 entry(self)，content 向后移动、last_key 变成当前未压缩 key */
    void decode (Slice& content, std::string& last_key);

    /* 按 key 进行大小比对 */
    bool operator < (const Entry& that) const;
private:
    Slice key_;     ///< 键
    Slice value_;   ///< 值

friend class TableBuilder;
};

#endif