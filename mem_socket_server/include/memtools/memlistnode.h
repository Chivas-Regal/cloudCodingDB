#pragma once

#include <cstdio>
#include <cstdint>

/**
 * @brief 空闲链表中的空闲节点
 * 
 * @details 组织出一条链表
 *          每个节点也要管理一个空闲区（首地址，大小）
 */
class MemListNode {
public:
    MemListNode *next; ///< 下一个指向空闲区的节点

    /* all(0) */
    MemListNode ();

    /* 构造(空闲区大小，空闲区首地址，下一个链表节点) */
    MemListNode (ssize_t _size, uint8_t* _true_add, MemListNode *_next);

    ~MemListNode () = default;

    /* 获取指向的缓冲区的首地址 */
    [[nodiscard]] const void *getAddress () const;

    /* 获取指向的缓冲区的大小 */
    [[nodiscard]] ssize_t  getSize () const;
private:
    ssize_t size;       ///< 管理空闲区的大小
    uint8_t* true_add;  ///< 管理空闲区的首地址

friend class MemList;
friend class MemList_FF;
friend class MemList_WF;
friend class MemList_BF;
};