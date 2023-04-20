#include "../../include/memtools/mempool.h"

#include <iostream>
#include <iomanip>

// @brief 内存池初始化
// @parma _nLists 空闲链表数量
//    新建 _nLists 个空闲链表, 每个空闲链表可用空间大小为 _oneSize
//    - _nLists*_oneSize is too big: failure
//    - else:						 successful
MemPool::MemPool(ssize_t _nLists, ssize_t _oneSize, int alloc_algorithm) :
		lists(new MemList*[_nLists]),
		sizeLists(_nLists),
		oneListSize(_oneSize) {
	std::set_new_handler(nullptr);
	beginPos = new uint8_t[_nLists * _oneSize];
	if (beginPos == nullptr) {
		std::cout << "Error: out of memory(in construct of MemPool)" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < _nLists; i ++) {
		// 虽然 lists_1, list_2, ... 连续，但是 list_1.head, list_2.head, ... 不连续
		if (alloc_algorithm == FIRST_FIT)
			lists[i] = new MemList_FF(beginPos + i * _oneSize, _oneSize);
		else if (alloc_algorithm == BEST_FIT)
			lists[i] = new MemList_BF(beginPos + i * _oneSize, _oneSize);
		else
			lists[i] = new MemList_WF(beginPos + i * _oneSize, _oneSize);
	}
}

// @brief 内存池析构
// 对size个空闲链表清空，再释放这个指针数组
MemPool::~MemPool() {
	for (int i = 0; i < sizeLists; i ++) {
		delete lists[i];
	}
	delete[] beginPos;
	delete[] lists;
}

// @brief 内存回归函数
//     先二分查到 address 属于哪个空闲链表
//     然后调用对应空闲链表的 deallocate 函数进行回收
// @parma size	  回归大小
// @parma address 回归首地址
void MemPool::deallocate(uint8_t *address, ssize_t _size) {
	int l = 0, r = (int)this->sizeLists - 1, res = 0;
	while (l < r) {
		int mid = (l + r) >> 1;
		if (address >= reinterpret_cast<uint8_t*>(lists[mid]->beginPos)) {
			l = mid + 1;
			res = mid;
		} else {
			r = mid - 1;
		}
	}
	if (address >= reinterpret_cast<uint8_t*>(lists[l]->beginPos))
		res = l;
	if (address >= reinterpret_cast<uint8_t*>(lists[r]->beginPos)) {
		res = r;
	}
	// 修改内存池，加锁
	mutex.lock();
	lists[res]->deallocate(address, _size);
	mutex.unlock();
}

// @brief 内存池打印
// 横向打印 sizeLists 张表，每张表都是表现了每个空闲链表的节点信息
void MemPool::print() const {
	std::cout << std::endl;
	std::cout.setf(std::ios::left);

	auto **p = new MemListNode*[sizeLists];
	for (int i = 0; i < sizeLists; i ++) {
		p[i] = lists[i]->head;
	}
	for (int i = 0; i < sizeLists; i ++)
		std::cout << "+-----------------------------+       ";
	std::cout << std::endl;
	for (int i = 0; i < sizeLists; i ++) {
		std::cout << "|  " << std::setw(16) << "Address" << "| " << std::setw(9) << "size" << "|";
		if (i != sizeLists - 1) {
			std::cout << " ----> ";
		}
	}
	std::cout << std::endl;
	for (int i = 0; i < sizeLists; i ++)
		std::cout << "+-----------------------------+       ";
	std::cout << std::endl;
	while (true) {
		bool hasOutput = false;
		for (int i = 0 ;i < sizeLists; i ++) {
			if (p[i]) {
				hasOutput = true;
			}
		}
		if (!hasOutput) break;
		for (int i = 0; i < sizeLists; i ++) {
			if (p[i]) {
				std::cout << "|  " << std::setw(16) << p[i]->getAddress() << "| " << std::setw(9) << p[i]->getSize() << "|       ";
			} else {
				std::cout << "                                      ";
			}
		}
		std::cout << std::endl;
		for (int i = 0; i < sizeLists; i ++) {
			if (p[i]) {
				std::cout << "+-----------------------------+       ";
				p[i] = p[i]->next;
			} else {
				std::cout << "                                      ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	delete[] p;
}

// @brief 表打印
// 打印内存池中第 i 张表
void MemPool::print(int i) const {
	lists[i]->print();
}

// @brief 内存配置
//     扫描所有链表头，根据记录的表内最大空闲块来判断该表是否能分配 _size 内存
//     如果可以，调用对应链表的 allocate(_size) 函数
// @parma _size 需求大小
// @return
//     - not nullptr: successfully
//     - nullptr:     failure
void *MemPool::allocate(ssize_t _size) {
	// 为防止幻读需加锁
	mutex.lock();
	for (int i = 0; i < this->sizeLists; i ++) {
		if (_size <= lists[i]->maxSize && lists[i]->maxSize - _size >= sizeof(MemListNode)) {
			void* ret = lists[i]->allocate(_size);
			mutex.unlock();
			return ret;
		}
	}
	mutex.unlock();
	return nullptr;
}

ssize_t MemPool::getSizeLists() const {
	return sizeLists;
}

ssize_t MemPool::getOneListSize() const {
	return oneListSize;
}