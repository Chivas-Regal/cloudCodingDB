#ifndef _MEMLIST_H_
#define _MEMLIST_H_

#include "memlistnode.h"

#include <cstdio>
#include <cstdint>
#include <set>

/** 
 * @brief 空闲内存链表
 * 
 * @details 内存从 MemList 中申请(new)，归还(delete)时释放回先前申请的 MemList 中
 */ 
class MemList {
public:
    MemList () = delete;

    /* 通过 pos 是否为 null 决定是否新开地址，新建 true_address, head, beginPos 
       (首地址，大小) */
    MemList (void* pos, ssize_t size);

    /* 根据 true_address 是否为新建的来决定是否 delete true_address */
    virtual ~MemList ();

    /* 空闲链表打印 */
	void print () const;

    /* 内存归还(首地址，归还大小) */
    void deallocate (uint8_t *address, ssize_t size);

    /* 刷新本空闲链表空闲节点最大值 */
    void resetMaxSize ();

    /* 分配地址（接口），为三种不同的算法子类声明函数 */
    virtual void* allocate (ssize_t size) = 0;

public:
    bool new_true_address;      ///< 实际内存区是否为新建
    uint8_t*     true_address;  ///< 实际内存区
    MemListNode* head;       	///< 头结点（含信息）
    ssize_t      maxSize;    	///< 可用最大空间链表节点
    void*        beginPos;   	///< 管理块的首地址
    ssize_t      beginSize;  	///< 申请时使用的空间

friend class MemPool;
};

#endif