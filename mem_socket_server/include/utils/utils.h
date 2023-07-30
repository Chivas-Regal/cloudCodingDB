#ifndef _INCLUDE_UTILS_UTILS_H_
#define _INCLUDE_UTILS_UTILS_H_

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include "slice.h"
#include "skiplist.h"

/* 判断本机字节序 */
int BitHighLow ();

/* 报错封装 */
void errif (bool, const char*);


/**
 * @brief 任意类型按字节顺序存入字符串并返回
 * 
 * @tparam T 要变成字符串的类型
 * @param value 待转变量
 * @return std::string value转成的字符串
 */
template<typename T>
std::string type_to_string (const T& value) {
    uint8_t const* ptr = (uint8_t*)&value;
    std::string ret;
    for (int i = 0; i < sizeof(T); i ++) {
        ret += char(*ptr);
        ptr ++; 
    }
    return ret;
}

/**
 * @brief 字符串按顺序翻译成T类型的字节，转变成T类型变量返回
 * 
 * @tparam T 字符串转变成的类型
 * @param s 待转字符串
 * @return T s转变成的T类型变量
 */
template<typename T>
T string_to_type (const std::string& s) {
    T* ret_pointer = new T();
    uint8_t* ptr = (uint8_t*)ret_pointer;
    for (int i = 0; i < sizeof(T) - s.size(); i ++) {
        *ptr = 0;
        ptr ++;
    }
    for (int i = std::max(0, (int)s.size() - (int)sizeof(T)); i < s.size(); i ++) {
        *ptr = int(s[i]);
        ptr ++;
    }
    T ret_value = *ret_pointer;
    delete ret_pointer;
    return ret_value;
}

/* 将 value 编码为 4 位字节后添加到 dst 后面 */
void setFixed32 (std::string* dst, uint32_t value);
/* 将 value 编码为 8 位字节后添加到 dst 后面 */
void setFixed64 (std::string* dst, uint64_t value);
/* 将 value 编码为变长字节（128进制，1~5B）后添加到 dst 后面 */
void setVarint32 (std::string* dst, uint32_t value);
/* 将 value 编码为变长字节（128进制，1~10B）后添加到 dst 后面 */
void setVarint64 (std::string* dst, uint64_t value);

/* 解码 src 的前 4 字节为 32位整数，并让 src 后移 4 位 */
uint32_t getFixed32 (Slice& src);
uint32_t getFixed32 (Slice&& src);
/* 解码 src 的前 8 字节为 64位整数，并让 src 后移 8 位 */
uint64_t getFixed64 (Slice& src);
uint64_t getFixed64 (Slice&& src);
/* 解码 src 的前 1~5B 字节为 32位整数，并让 src 后移对应位数 */
uint32_t getVarint32 (Slice& src);
uint32_t getVarint32 (Slice&& src);
/* 解码 src 的前 1~10B 字节为 64位整数，并让 src 后移对应位数 */
uint64_t getVarint64 (Slice& src);
uint64_t getVarint64 (Slice&& src);

/**
 * @brief 从 SkipList 派生出来的适用于 {(std::string)Key : (T)value} 的存储
 * 
 * @tparam T 值的类型
 * 
 * @details 严格对 key 要求唯一，所以对 pair<k,v> 排序和对 k 排序一样，与 v 无关
 */
template<typename T>
class KV_SkipList : public SkipList<std::pair<std::string, T>> {
public:
    /* 通过 key 寻找出对应的 value */
    T get (const std::string& key);
    /* 向键值对跳表内存入一个 {key, value} */
    void put (const std::string& key, const T& value);
    /* 获取 key 是否存在 */
    bool count (const std::string& key);
    /* 获取所有的键值对 */
    std::vector<std::pair<std::string, T>> getKV();
};

/**
 * @brief 通过 key 寻找出对应的 value
 * 
 * @param key 设置求值的键
 * @return T 找到的值
 */
template<typename T> T
KV_SkipList<T>::get (const std::string& key) {
    std::shared_ptr<__SkipListNode<std::pair<std::string, T>>> upeq_node = this->upper_equal({key, T()});
    // 大于等于 key 的如果不是 key ，说明没有
    if (upeq_node && upeq_node->value.first == key)
        return upeq_node->value.second;
    else
        return T();
}
/**
 * @brief 向键值对跳表内存入一个 {key, value}
 * 
 * @param key   键
 * @param value 值
 */
template<typename T> void 
KV_SkipList<T>::put (const std::string& key, const T& value) {
    std::shared_ptr<__SkipListNode<std::pair<std::string, T>>> lw_node = this->lower({key, T()});
    // 如果大于等于的位置上的 key = key 说明已经存在，直接修改
    if (lw_node->right && lw_node->right->value.first == key) 
        lw_node->right->value.second = value;
    // 否则通过最大的小于 key 的位置右侧插入一个
    else
        this->insert({key, value}, lw_node);
}
/**
 * @brief 获取 key 是否存在
 * 
 * @param key   键
 * 
 * @return true 存在
 * @return false 不存在
 */
template<typename T> bool 
KV_SkipList<T>::count (const std::string& key) {
    std::shared_ptr<__SkipListNode<std::pair<std::string, T>>> upeq_node = this->upper_equal({key, T()});
    if (upeq_node && upeq_node->value.first == key) 
        return true;
    else
        return false;
}

/**
 * @brief 获取所有的键值对
 * 
 * @tparam T 值的类型
 * @return std::vector<std::pair<std::string, T>> 键值对集合
 */
template<typename T> std::vector<std::pair<std::string, T>> 
KV_SkipList<T>::getKV () {
    std::vector<std::pair<std::string, T>> ret;
    std::shared_ptr<__SkipListNode<std::pair<std::string, T>>> p = this->head;
    while (p->down) {
        p = p->down;
    }
    p = p->right;
    while (p) {
        ret.push_back(p->value);
        p = p->right;
    }
    return ret;
}

#endif