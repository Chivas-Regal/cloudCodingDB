#ifndef _MEMLIST_H_
#define _MEMLIST_H_

#include "memlistnode.h"

#include <cstdio>
#include <cstdint>
#include <set>

// @brief 空闲内存链表
// 内存从 MemList 中申请(new)，归还(delete)时释放回先前申请的 MemList 中
class MemList {
public:
    MemList () = delete;
    MemList (void* pos, ssize_t size);
    virtual ~MemList ();

	void print () const;
    void deallocate (uint8_t *address, ssize_t size);
    void resetMaxSize ();

    virtual void* allocate (ssize_t size) = 0;

public:
    MemListNode* head;       	///< 头结点（含信息）
    ssize_t      maxSize;    	///< 可用最大空间链表节点
    void*        beginPos;   	///< 管理块的首地址
    ssize_t      beginSize;  	///< 申请时使用的空间

friend class MemPool;
};

#endif