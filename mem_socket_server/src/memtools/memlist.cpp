#include "memlist.h"
#include <iomanip>
#include <iostream>

/** 
 * @brief 构造函数
 * @param pos   内存块首地址
 *      - nullptr:  开辟新地址
 *      - else:     用传进来的地址
 * @param size  内存块大小
 * 
 * @details 若使用传进来的地址，new_true_address 为 false ，否则为 true
 */ 
MemList::MemList(void* pos, ssize_t size): maxSize(size), beginSize(size) {
    if (pos == nullptr)
        true_address = new uint8_t[size],
        new_true_address = true;
    else    
        true_address = reinterpret_cast<uint8_t*>(pos),
        new_true_address = false;
    head = new MemListNode(size, true_address, nullptr);
    beginPos = pos;
}

/** 
 * @brief 析构函数
 *      - safe:   地址全部被归还，一次性清空
 *      - unsafe: 地址部分被归还，遍历链表清空（Warning）
 * 
 * @details 一定会删掉链表内所有节点
 *           如果链表管理的内存是自己建的，就删掉这段内存
 */ 
MemList::~MemList() {
    if (head->size < beginSize) {
        std::cout << "\033[33;1mWarning\033[0m: some memory still not reclaimed (in MemList destructors)" << std::endl;
	}
    MemListNode* p = head;
    while (p) {
        MemListNode *t = p;
        p = p->next;
        delete t;
    }
    if (new_true_address)
        delete true_address;
}

/** 
 * @brief 链表打印
 * 
 * @details 表格形式，表头：|-链表节点首地址-|-链表节点大小-|
 */ 
[[maybe_unused]] void MemList::print() const {
    MemListNode *p = head;
	std::cout.setf(std::ios::left);
    std::cout << "+-----------------------------+" << std::endl;
	std::cout << "|  " << std::setw(16) << "Address" << "| " << std::setw(9) << "size" << "|" << std::endl;
    std::cout << "+-----------------------------+" << std::endl;
    while (p) {
		std::cout << "|  " << std::setw(16) << p->getAddress() << "| " << std::setw(9) << p->getSize() << "|" << std::endl;
        p = p->next;
    }
    std::cout << "+-----------------------------+" << std::endl << std::endl;
}

/** 
 * @brief 内存归还
 *      情况：
 *        - 1.可与前面合并
 *        - 2.可与前面和后面合并
 *        - 3.可与后面合并
 *        - 4.自己独立
 * 
 * @param address 归还首地址
 * @param size      归还大小
 */
void MemList::deallocate(uint8_t *address, ssize_t size) {
    /* 回收作为头结点 */
    if (!head) { // 没有头结点
        head = new MemListNode(size, address, nullptr);
        return;
    } else if (address < head->getAddress()) { 
        if (address + size == head->getAddress()) { // 可与后面的头结点合并，应该删掉原来的头结点
            MemListNode* t_head = head;
            head = new MemListNode(size + head->size, address, head->next);
            delete t_head;
        } else { // 补新的头结点
            head = new MemListNode(size, address, head);
        }
        return;
    }
    MemListNode *p = head;
    while (p) {
        if ((p->next && address < p->next->getAddress()) || !p->next) { // 在 p 后面插入
            if ((uint8_t*)p->getAddress() + p->size == address) {           // 情况1
                p->size += size;
                if (p->next && address + size == p->next->getAddress()) {   // 情况2（要删除一个）
                    MemListNode* beforeNext = p->next;
                    p->size += p->next->size;
                    p->next = p->next->next;
                    delete beforeNext;
                }
                maxSize = std::max(maxSize, p->size);
            } else if (p->next && address + size == p->next->getAddress()) {// 情况3
                p->true_add = address;
                p->size = size + p->size;
                maxSize = std::max(maxSize, p->size);
            } else {                                                        // 情况4
                p->next = new MemListNode(size, address, p->next);
                maxSize = std::max(maxSize, size);
            }
            return;
        }
        p = p->next;
    }
}

/** 
 * @brief 更新最大可用空间
 * 
 * @details O(n)扫描所有节点，maxSize 记录最大值
 */
void MemList::resetMaxSize() {
    this->maxSize = 0;
    MemListNode *p = head;
    while (p) {
        this->maxSize = std::max(this->maxSize, p->size);
        p = p->next;
    }
}