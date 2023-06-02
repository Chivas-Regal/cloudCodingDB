#pragma once

#include "memlist.h"
#include "memlistnode.h"
#include <cstdint>

/** 
 * @brief WF算法派生MemList
 * 
 * @details 最坏适应，将扫描到的足够大小的最小的空闲节点向外配置
 */ 
class MemList_WF : public MemList {
public:
    MemList_WF () = delete;
    MemList_WF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_WF() override = default;

    /* 重写 MemList::allocate，选择整个链表中最小的可分配空闲区 */
    void* allocate (ssize_t size) override;
};
