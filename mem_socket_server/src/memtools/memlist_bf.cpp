#include "../../include/memtools/memlist_bf.h"

void *MemList_BF::allocate(ssize_t size) {
    MemListNode *p = head;
    ssize_t mxSize = -1;
    MemListNode *retNode;
    while (p) {
        if (p->size >= size && p->size - size >= sizeof(MemListNode)) {
            if (mxSize == -1 || mxSize > p->size) {
                mxSize = p->size;
                retNode = p;
            }
        }
        p = p->next;
    }

    if (mxSize == -1) return nullptr;
    else {
        retNode->size -= size;
        resetMaxSize();
        return (uint8_t *)retNode->getAddress() + retNode->size;
    }
}