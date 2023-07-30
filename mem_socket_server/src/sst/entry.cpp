#include "entry.h"
#include "utils.h"
#include <assert.h>

/**
 * @brief 空构造，key=value=""
 */
Entry::Entry() : key_(), value_() {}

/**
 * @brief 利用已有的字符串 key,value 构造
 * 
 * @param key 已有键
 * @param value 已有值
 */
Entry::Entry(const char* key, const char* value) : key_(key), value_(value) {}
/**
 * @brief 利用已有的字符串 key,value 构造
 * 
 * @param key 已有键
 * @param value 已有值
 */
Entry::Entry(const std::string& key, const std::string& value) : key_(key), value_(value) {}
/**
 * @brief 利用已有的字符串 key,value 构造
 * 
 * @param key 已有键
 * @param value 已有值
 */
Entry::Entry(std::string&& key, std::string&& value) : key_(key), value_(value) {}

/**
 * @brief 获取 utf-8 未压缩键
 * 
 * @return std::string 未压缩键
 */
std::string Entry::key() {
    return key_.toString();
}

/**
 * @brief 获取 utf-8 值
 * 
 * @return std::string 值
 */
std::string Entry::value() {
    return value_.toString();
}

/**
 * @brief 利用上一个的完整 key 进行编码
 * 
 * @param last_key 上一个键值对的完整 key 
 * @return std::string 编码后的字节字符串
 * 
 * shared_bytes、unshared_bytes、value_size、key_delta、value 均对应 entry.h 的结构图信息。
 */
std::string Entry::encode(const std::string& last_key) {
    /* 获取共享前缀长度 */
    size_t shared_bytes = 0;
    while (shared_bytes < last_key.size() && shared_bytes < key_.size()) {
        if (last_key[shared_bytes] == key_[shared_bytes])
            shared_bytes ++;
        else
            break;
    }
    size_t unshared_bytes = key_.size() - shared_bytes;
    size_t value_length = value_.size();
    Slice key_delta = key_;
    key_delta.remove_prefix(shared_bytes);

    /* 将三个信息编入 ret_s 后返回带上压缩键和值的编码 */
    std::string ret_s;
    setVarint32(&ret_s, shared_bytes);
    setVarint32(&ret_s, unshared_bytes);
    setVarint32(&ret_s, value_length);
    return ret_s + 
           key_delta.toString() + 
           value_.toString();
}

/**
 * @brief 利用上一个完整 key 在 content 前缀中解码出一个 entry ，并让 entry 删去这个前缀，last_key 变化为未解码 key
 * 
 * @param content 前缀含 entry 的首部字符串
 * @param last_key 上一个 entry 的未压缩 key
 * 
 * shared_bytes、unshared_bytes、value_size、key_delta、value 均对应 entry.h 的结构图信息。
 */
void Entry::decode(Slice& content, std::string& last_key) {
    /* 获取三个动态长度的变量元素 */
    size_t shared_bytes = getVarint32(content);
    size_t unshared_bytes = getVarint32(content);
    size_t value_size = getVarint32(content);

    assert(content.size() >= unshared_bytes + value_size);

    /* 解析出压缩 key 和 value */
    std::string key_delta = content.toString().substr(0, unshared_bytes);
    content.remove_prefix(unshared_bytes);
    std::string value = content.toString().substr(0, value_size);
    content.remove_prefix(value_size);

    /* 利用压缩的长度切出前缀，并在后面加入压缩 key 的内容 */
    last_key = last_key.substr(0, shared_bytes);
    last_key.append(key_delta);
    key_ = last_key;
    value_ = value;
}

/**
 * @brief 按 key 大小进行比对
 * 
 * @param that 待比较放在右侧的键值对
 * @return true  this.key <  that.key
 * @return false this.key >= that.key
 */
bool Entry::operator < (const Entry & that) const {
    return key_.compare(that.key_) == -1;
}
