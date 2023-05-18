#include "memlist_bf.h"

/**
 * @brief 空间分配
 * 
 * @details 找到最小的可分配的空闲节点
 * 
 * @param size   申请大小
 * @return void* 返回首地址
 *          - nullptr     分配失败
 *          - not nullptr 分配成功
 */
void* MemList_BF::allocate(ssize_t size) {
    MemListNode *fap = nullptr; // *p 的前驱
    MemListNode *p = head;      // 遍历链表用
    ssize_t mn_size = -1;       // 最小的空闲区大小
    MemListNode *fa_retnode;    // 返回节点的前驱
    MemListNode *retnode;       // 返回节点
    while (p) {
        if (p->size >= size) { // 足够分配
            if (mn_size == -1 || mn_size > p->size) { // 维护最小节点
                mn_size = p->size;
                fa_retnode = fap;
                retnode = p;
            }
        }
        fap = p;
        p = p->next;
    }

    if (mn_size == -1) return nullptr;
    else {
        void* ret = retnode->true_add;
        if (retnode->size == size) { // 刚好被分完，要删除掉 retnode
            if (fa_retnode) {
                fa_retnode->next = retnode->next;
            } else {
                head = head->next;
            }
            delete retnode;
        } else { // 删除不掉，变动一下 空闲区大小和首地址 即可
            retnode->size -= size;
            retnode->true_add += size;
        }
        resetMaxSize();
        return ret;
    }
}