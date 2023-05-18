#include "memlistnode.h"

MemListNode::MemListNode ():
    next(nullptr),
    true_add(nullptr),
    size(0)
{}

/**
 * @brief 带值构造函数
 * 
 * @param _size      空闲区大小
 * @param _true_add  空闲区首地址
 * @param _next      下一个指向空闲区的空闲节点
 */
MemListNode::MemListNode (ssize_t _size, uint8_t* _true_add, MemListNode *_next):
    size(_size),
    true_add(_true_add),
    next(_next)
{}

/**
 * @brief 获取指向空闲区的首地址
 * 
 * @return const void* 首地址
 */
[[nodiscard]] const void* MemListNode::getAddress () const {
    return this->true_add;
}

/**
 * @brief 获取指向空闲区大小
 * 
 * @return ssize_t 大小
 */
[[nodiscard]] ssize_t  MemListNode::getSize () const {
    return size;
}