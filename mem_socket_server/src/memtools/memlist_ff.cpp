#include "memlist_ff.h"

/**
 * @brief 空间分配
 * 
 * @details 找到地址最小的可分配的空闲节点
 * 
 * @param size   申请大小
 * @return void* 返回首地址
 *          - nullptr     分配失败
 *          - not nullptr 分配成功
 */
void* MemList_FF::allocate(ssize_t size) {
    MemListNode *fa = nullptr;
    MemListNode *p = head;
    while (p) {
        if (p->size >= size) { // 找到了
            void* ret = p->true_add;
            if (p->size == size) { // 刚好分完，要删掉这个节点
                if (fa)
                    fa->next = p->next;
                else
                    head = head->next;
                delete p;
            } else { // 没有分完，就正常的修改一下空闲区的首地址和大小即可
                p->true_add += size;
                p->size -= size;
            }
            resetMaxSize();
            return ret;
        }
        // fa 和 p 都往后走
        fa = p;
        p = p->next;
    }
    return nullptr;
}
