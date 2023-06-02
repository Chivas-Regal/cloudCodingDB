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

    /* 内存池初始化(空闲链表数量，单个空闲区大小，分区分配算法) */
    MemPool (ssize_t nLists, ssize_t oneSize, int alloc_algorithm);

    /* 清空 beginPos, list[0~sizeLists], list */
    ~MemPool ();

    /* 打印所有的空闲链表信息 */
    void    print() const;

    /* 打印第 i 个空闲链表信息 */
	void  	print(int i) const;

    /* 回收一个首地址是 address， _size 个字节的空间 */
    void    deallocate (uint8_t *address, ssize_t _size);

    /* 分配一个大小为 _size 个字节的内存 */
    void*   allocate (ssize_t _size);

    /* 获取空闲链表数量 */
    [[nodiscard]] ssize_t getSizeLists() const;

    /* 获取每个空闲链表管理的分区大小 */
    [[nodiscard]] ssize_t getOneListSize () const;
public:
    ssize_t       sizeLists;   ///< 空闲链表的数量
    ssize_t       oneListSize; ///< 每个空闲链表的大小
	uint8_t*      beginPos;	   ///< 内存池真正的起始位置
    MemList**  	  lists;	   ///< 空闲链表们
	std::mutex 	  mutex;	   ///< 操作空闲链表的互斥锁
};

#endif