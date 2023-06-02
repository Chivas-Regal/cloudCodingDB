#pragma once

#include "memlist.h"
#include "memlistnode.h"

/** 
 * @brief BF算法派生MemList
 * 
 * @details 最佳适应，将扫描到的足够大小的最大的空闲节点向外配置
 */ 
class MemList_BF : public MemList {
public:
    MemList_BF () = delete;
    MemList_BF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_BF() override = default;

    /* 重写 MemList::allocate，选择整个链表中最大的可分配空闲区 */
    void* allocate (ssize_t size) override;
};
