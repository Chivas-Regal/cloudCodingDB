#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "memlist_ff.h"
#include "memlist_bf.h"
#include "memlist_wf.h"

#include <string>
#include <mutex>

#define FIRST_FIT 1
#define BEST_FIT  2
#define WORST_FIT 3

// @brief 内存池
// 动态分区管理连续内存
class MemPool {
public:
    MemPool () = delete;
    MemPool (ssize_t nLists, ssize_t oneSize, int alloc_algorithm);
    ~MemPool ();

    void    print() const;
	void  	print(int i) const;
    void    deallocate (uint8_t *address, ssize_t _size);
    void*   allocate (ssize_t _size);

    [[nodiscard]] ssize_t getSizeLists() const;
    [[nodiscard]] ssize_t getOneListSize () const;
public:
    ssize_t       sizeLists;   ///< 空闲链表的数量
    ssize_t       oneListSize; ///< 每个空闲链表的大小
	uint8_t*      beginPos;	   ///< 内存池真正的起始位置
    MemList**  	  lists;	   ///< 空闲链表们
	std::mutex 	  mutex;	   ///< 操作空闲链表的互斥锁
};

#endif