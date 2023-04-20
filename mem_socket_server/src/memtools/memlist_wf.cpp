#include "../../include/memtools/memlist_wf.h"
void *MemList_WF::allocate(ssize_t size) {
    MemListNode *p = head;
    int mxSize = -1;
    MemListNode *retNode;
    while (p) {
        if (p->size >= size && p->size - size >= sizeof(MemListNode)) {
            if (mxSize < p->size) {
                mxSize = (int)p->size;
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



