#ifndef _MEMLIST_FF_H_
#define _MEMLIST_FF_H_

#include "memlistnode.h"
#include "memlist.h"

// @brief FF算法派生MemList
// 首次适应，将扫描到的第一个空间足够的空闲节点向外配置
class MemList_FF : public MemList {
public:
    MemList_FF () = delete;
    MemList_FF (void* pos, ssize_t size) : MemList(pos, size) {}
    ~MemList_FF() override = default;

    void* allocate (ssize_t size) override;
};

#endif