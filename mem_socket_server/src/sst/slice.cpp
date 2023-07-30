#include "slice.h"
#include <assert.h>
#include <cstring>

/**
 * @brief 默认初始化，空字符串
 */
Slice::Slice() : data_("") {}

/**
 * @brief 用已有字符串 s 构造
 */
Slice::Slice(const char* s) : data_(s) {}

/**
 * @brief 用已有字符串 s 构造
 */
Slice::Slice(const std::string& s) : data_(s) {}

/**
 * @brief 用已有字符串 s 构造
 */
Slice::Slice(std::string&& s) : data_(s) {}

/**
 * @brief 获取 data_
 * 
 * @return std::string 非引用返回 this.data_ 
 */
std::string Slice::data() const {
    return data_;
}

/**
 * @brief data_ 的长度
 * 
 * @return size_t data_.size()
 */
size_t Slice::size() const {
    return data_.size();
}

/**
 * @brief 清空自己所表示的字符串
 */
void Slice::clear() {
    data_ = "";
}

/**
 * @brief 判断自己是否为空
 * 
 * @return true 空
 * @return false 非空
 */
bool Slice::empty () const {
    return size() == 0;
}

/**
 * @brief data_ 去掉前 n 个字符（后移 n 位）
 * 
 * @param n 后移的位数
 */
void Slice::remove_prefix (size_t n) {
    assert(n <= size());
    data_ = data_.substr(n);
}

/**
 * @brief 判断 that 是否为 this 的前缀
 * 
 * @param that 被判断是否为 this 前缀的字符串
 * @return true 是
 * @return false 不是
 */
bool Slice::starts_with(const Slice& that) const {
    return size() >= that.size() && data_.substr(0, that.size()) == that.data_;
}

/**
 * @brief 比对 this 和 that 的大小
 * 
 * @param that 右侧待比对字符串
 * @return int 比对结果
 *     @retval -1 this<that
 *     @retval  0 this=that
 *     @retval  1 this>that
 */
int Slice::compare(const Slice& that) const {
    int min_size = std::min(size(), that.size());
    for (int i = 0; i < min_size; i ++) {
        if (operator[](i) != that[i]) {
            if (operator[](i) < that[i]) 
                return -1;
            else
                return 1;
        }
    }
    if (size() > that.size())
        return 1;
    else if (size() == that.size())
        return 0;
    else 
        return -1;
}

/**
 * @brief 返回字符串形式
 * 
 * @return std::string 自己本身所表示的字符串
 */
std::string Slice::toString() const {
    return data_;
}

/**
 * @brief 返回自己的第 index 位字符
 * 
 * @param index 字符位置
 * @return char 该位置上的字符
 */
char Slice::operator [] (int index) const {
    assert(index < size());
    return data_[index];
}