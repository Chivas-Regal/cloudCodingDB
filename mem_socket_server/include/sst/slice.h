#ifndef _INCLUDE_SST_SLICE_H_
#define _INCLUDE_SST_SLICE_H_

#include <string>

class Slice {
public: 
    Slice ();
    Slice (const char* s);
    Slice (const std::string& s);
    Slice (std::string&& s);

    Slice (const Slice& that) = default;
    Slice& operator = (const Slice& that) = default;
    ~Slice() = default;
public:
    /* 获取 data_ */
    std::string data () const;

    /* 获取 data_ 的长度 */
    size_t size () const;

    /* 清空 data_ */
    void clear ();

    /* 判断 data_ 是否为空 */
    bool empty () const;

    /* data_ 去掉前 n 个字符（后移 n 位） */
    void remove_prefix (size_t n);

    /* 判断 that 是否可以作为 this 的前缀 */
    bool starts_with (const Slice& that) const;

    /* 比对 this 和 that 的大小 */
    int compare (const Slice& that) const;

    /* 获取 data_ */
    std::string toString () const;

    /* 获取 data_ 第 index 位 */
    char operator[] (int index) const;
private:
    std::string data_;
};

#endif