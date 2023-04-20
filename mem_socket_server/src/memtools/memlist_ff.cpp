#include "../../include/memtools/memlist_ff.h"

// @brief 空间分配函数
// @parma size 需要分配的空间大小
// @return
//   - not nullptr:  存在空间足够的节点（分配后剩余空间还要能保存一个空闲节点）
//   - nullptr:      空间都不够分配
void* MemList_FF::allocate(ssize_t size) {
    MemListNode *p = head;
    while (p) {
        if (p->size >= size && p->size - size >= sizeof(MemListNode)) {
            p->size -= size;
            resetMaxSize();
            return (uint8_t *)p->getAddress() + p->size;
        }
        p = p->next;
    }
    return nullptr;
}

