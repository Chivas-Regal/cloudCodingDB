#pragma once

#include "memlist.h"
#include "memlistnode.h"

/** 
 * @brief FF算法派生MemList
 * 
 * @details 首次适应，将扫描到的第一个空间足够的空闲节点向外配置
 */ 
class MemList_FF : public MemList {
public:
    MemList_FF () = delete;
    MemList_FF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_FF() override = default;

    /* 重写 MemList::allocate，选择整个链表中最靠前的可分配空闲区 */
    void* allocate (ssize_t size) override;
};