#include "../include/memtools/mempool.h"
#include <assert.h>

int main () {
    MemPool *mempool = new MemPool(1000, 10000, FIRST_FIT);

    uint8_t* ptr1 = reinterpret_cast<uint8_t*>(mempool->allocate(9900 * sizeof(uint8_t)));
    uint32_t *ptr2 = reinterpret_cast<uint32_t*>(mempool->allocate(10 * sizeof(uint32_t)));
    uint8_t* ptr3 = reinterpret_cast<uint8_t*>(mempool->allocate(9900 * sizeof(uint8_t)));
    mempool->deallocate((uint8_t*)ptr2, 10 * sizeof(uint32_t));
    uint64_t *ptr4 = reinterpret_cast<uint64_t*>(mempool->allocate(10 * sizeof(uint64_t)));

    int true_rels =   1000 * 10000 
                    - 9900 * sizeof(uint8_t)
                    - 10 * sizeof(uint32_t)
                    - 9900 * sizeof(uint8_t)
                    + 10 * sizeof(uint32_t)
                    - 10 * sizeof(uint64_t);

    int real_rels = 0;
    for (int i = 0; i < 1000; i ++) {
        MemListNode *p = mempool->lists[i]->head;
        while (p) {
            real_rels += p->getSize();
            p = p->next;
        }
    }
    assert(real_rels == true_rels);
}