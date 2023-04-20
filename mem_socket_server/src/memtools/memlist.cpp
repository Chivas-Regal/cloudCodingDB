#include "../../include/memtools/memlist.h"

#include <iomanip>
#include <iostream>

// @brief 构造函数
// @parma pos   内存块首地址
//      - nullptr:  开辟新地址
//      - else:     用传进来的地址
// @parma size  内存块大小
//      - size < sizeof(MemListNode): failure
//      - else:                       successful
MemList::MemList(void* pos, ssize_t size): maxSize(size), beginSize(size) {
    if (size < sizeof(MemListNode)) {
        std::cout << "Error: too small to be free-list-node" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pos == nullptr)
        pos = new uint8_t[size];
    head = reinterpret_cast<MemListNode*>(pos);
    new(head) MemListNode(size, nullptr);
    beginPos = pos;
}

// @brief 析构函数
//      - safe:   地址全部被归还，一次性清空
//      - unsafe: 地址部分被归还，遍历链表清空（Warning）
MemList::~MemList() {
    if (head->size < beginSize) {
        std::cout << "\033[33;1mWarning\033[0m: some memory still not reclaimed (in MemList destructors)" << std::endl;
	}
}

// @brief 链表打印
// 表格形式，表头：|-链表节点首地址-|-链表节点大小-|
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

// @brief 内存归还
//      情况：
//        - 1.可与前面合并
//        - 2.可与前面和后面合并
//        - 3.可与后面合并
//        - 4.自己独立
// @parma address   归还首地址
// @parma size      归还大小
void MemList::deallocate(uint8_t *address, ssize_t size) {
    MemListNode *p = head;
    while (p) {
        if ((p->next && address < p->next->getAddress()) || !p->next) { // 在 p 后面插入
            if ((uint8_t *)p->getAddress() + p->size == address) {          // 情况1
                p->size += size;
                if (p->next && address + size == p->next->getAddress()) {   // 情况2
                    p->size += p->next->size;
                    p->next = p->next->next;
                }
                maxSize = std::max(maxSize, p->size);
            } else if (p->next && address + size == p->next->getAddress()) {// 情况3
                ssize_t beforeNextSize = p->next->getSize();
                MemListNode* beforeNextNext = p->next->next;
                p->next = (MemListNode*)address;
                new(p->next) MemListNode(beforeNextSize + size, beforeNextNext);
                maxSize = std::max(maxSize, beforeNextSize + size);
            } else {                                                        // 情况4
                MemListNode* nextBefore = p->next;
                // 同理 placement new
                p->next = reinterpret_cast<MemListNode*>(address);
                new(p->next) MemListNode(size, nextBefore);
                maxSize = std::max(maxSize, size);
            }
            return;
        }
        p = p->next;
    }
}

// @brief 更新最大可用空间
// O(n)扫描所有节点，maxSize 记录最大值
void MemList::resetMaxSize() {
    this->maxSize = 0;
    MemListNode *p = head;
    while (p) {
        this->maxSize = std::max(this->maxSize, p->size);
        p = p->next;
    }
}