#ifndef _INCLUDE_CONTAINERS_SKIPLIST_H_
#define _INCLUDE_CONTAINERS_SKIPLIST_H_

#include <queue>
#include <stack>
#include <random>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>

template<typename T> class SkipList;
template<typename T> struct __SkipListNode;


/* 跳表节点 */
template<typename T>
struct __SkipListNode {
    std::shared_ptr<__SkipListNode<T>> right; ///< 同层右侧节点
    std::shared_ptr<__SkipListNode<T>> down;  ///< 同列下层节点
    T value;             ///< 节点所带值
    bool is_head;        ///< 是否为不表示任何含义的头
    /* 可带两个指针初始化，也可以直接初始化令两指针为 null */
    __SkipListNode (const T& value_, bool is_head_) : right(nullptr), down(nullptr), value(value_), is_head(is_head_) {}
    __SkipListNode (std::shared_ptr<__SkipListNode<T>> r_, std::shared_ptr<__SkipListNode<T>> d_, const T& value_, bool is_head_) : right(r_), down(d_), value(value_), is_head(is_head_) {}
    ~__SkipListNode () = default;
};

template<typename T>
class SkipList {
protected: 
    std::shared_ptr<__SkipListNode<T>> head;
    size_t size_;
public:

    /* 构造，生成一个只有一层的 head 节点 */
    SkipList ();

    /* 析构，删除每一层的每一个节点 */
    ~SkipList ();

    /* 从 head 开始向后找到合适位置后插入 */
    void insert (const T& value);

    /* 从 begin 开始向后找到合适位置后插入 */
    void insert (const T& value, std::shared_ptr<__SkipListNode<T>> begin);

    /* 删除一个数据为 value 的元素 */
    bool erase (const T& value);

    /* 找到 >=value 的最小节点 */
    std::shared_ptr<__SkipListNode<T>> upper_equal (const T& value);

    /* 找到 <value 的最大节点 */
    std::shared_ptr<__SkipListNode<T>> lower (const T& value);

    /* 找到 <=value 的最大节点*/
    std::shared_ptr<__SkipListNode<T>> lower_equal (const T& value);
    
    /* 判断是否存在数据为 value 的元素 */
    bool count(const T& value);

    /* 获取数值个数 */
    size_t size ();

    /* 清空节点 */
    void clear ();

    /* 获取所有元素 */
    std::vector<T> getElements ();
};

/**
 * @brief 构造
 * 
 * @details 生成一个只有一层的 head 节点
 */
template<typename T>
SkipList<T>::SkipList () : head(new __SkipListNode<T>(T(), 1)), size_(0) {}

/**
 * @brief 析构
 * 
 * @details 删除每一层的每一个节点
 */
template<typename T>
SkipList<T>::~SkipList () {
    clear();
}
/**
 * @brief 从 head 开始向后找到合适位置后插入
 * 
 * @param value 插入元素的数值
 */
template<typename T> void
SkipList<T>::insert (const T& value) {
    insert(value, head);
}

/**
 * @brief 从 begin 开始向后找到合适位置后插入
 * 
 * @param value 插入元素的数值
 * @param begin 寻址起点
 * 
 * 在每一层找到不大于  value 的最后一个节点
 * 从最下面一层利用找到的这些节点开始插入，并抛硬币判断是否往上加，加了就再插入
 * 如果层数过多超过 head 的层数，将 head 也跟着向上加层
 */
template<typename T> void
SkipList<T>::insert (const T& value, std::shared_ptr<__SkipListNode<T>> begin) {
    size_ ++;
    // node.value<=value 所形成的一个墙（ path 内每一个都是每层 <=value 的最后一个节点）
    std::stack<std::shared_ptr<__SkipListNode<T>>> path;
    std::shared_ptr<__SkipListNode<T>> p = begin;
    while (p) {
        while (p->right && p->right->value <= value)
            p = p->right;
        path.push(p);
        p = p->down;
    }
    // 不用插入了，有同节点了
    if (!path.empty() && !path.top()->is_head && path.top()->value == value)
        return;
    // 向上抛硬币加层
    bool coin_bool = true;
    std::shared_ptr<__SkipListNode<T>> down_node = nullptr;
    while (!path.empty() && coin_bool) {
        path.top()->right = std::make_shared<__SkipListNode<T>>(path.top()->right, down_node, value, 0);
        down_node = path.top()->right;
        path.pop();
        coin_bool = (std::rand() & 1) == 0;
    }
    if (coin_bool) {
        head = std::make_shared<__SkipListNode<T>>(std::make_shared<__SkipListNode<T>>(nullptr, down_node, value, 0), head, T(), 1);
    }
}

/**
 * @brief 删除一个数据为 value 的元素
 * 
 * @param value 待删除数据
 * 
 * @return 删除的成功情况
 *   @retval true: 成功删除
 *   @retval false: 不存在，无法删除
 */
template<typename T> bool
SkipList<T>::erase (const T& value) {
    size_ --;
    bool ret = false;
    std::shared_ptr<__SkipListNode<T>> p = head;
    while (p) {
        while (p->right && p->right->value < value)
            p = p->right;
        if (p->right && p->right->value == value) {
            p->right = p->right->right;
            ret = true;
        }
        p = p->down;
    }
    return ret;
}

/**
 * @brief 找到 >=value 的最小节点
 * 
 * @param value 寻节点的标靶
 * @return SkipListNode* 返回找到的满足要求的节点地址
 */
template<typename T> std::shared_ptr<__SkipListNode<T>> 
SkipList<T>::upper_equal (const T& value) {
    return lower(value)->right;
}

/**
 * @brief 找到 <value 的最大节点
 * 
 * @param value 寻节点的标靶
 * @return SkipListNode* 返回找到的满足要求的节点地址
 */
template<typename T> std::shared_ptr<__SkipListNode<T>> 
SkipList<T>::lower (const T& value) {
    std::shared_ptr<__SkipListNode<T>> p = head;
    while (true) {
        while (p->right && p->right->value < value)
            p = p->right;
        if (!p->down) 
            break;
        p = p->down;
    }
    return p;
}

/**
 * @brief 找到 <=value 的最大节点
 * 
 * @param value 寻节点的标靶
 * @return SkipListNode* 返回找到的满足要求的节点地址
 */
template<typename T> std::shared_ptr<__SkipListNode<T>> 
SkipList<T>::lower_equal (const T& value) {
    std::shared_ptr<__SkipListNode<T>> p = head;
    while (true) {
        while (p->right && p->right->value <= value)
            p = p->right;
        if (!p->down) 
            break;
        p = p->down;
    }
    return p;
}

/**
 * @brief 判断是否存在数据为 value 的元素
 * 
 * @param value 寻找的数值
 * @return true  找到了
 * @return false 没找到
 */
template<typename T> bool
SkipList<T>::count(const T& value) {
    std::shared_ptr<__SkipListNode<T>> p = head;
    while (p) {
        while (p->right && p->right->value <= value) 
            p = p->right;
        if (!p->is_head && p->value == value) 
            return true;
        p = p->down;
    }
    return false;
}

/**
 * @brief 获取数值元素个数
 * 
 * @return size_t 获取到的个数
 */
template<typename T> size_t
SkipList<T>::size() {
    return size_;
}

/**
 * @brief 清空节点
 * 
 * 从最上面一层开始，释放完一层所有节点后进入下一层
 * 直到彻底清除完毕为止
 */
template<typename T> void
SkipList<T>::clear() {
    while (head) {
        std::shared_ptr<__SkipListNode<T>> lev_head = head;
        head = head->down;
        while (lev_head) {
            std::shared_ptr<__SkipListNode<T>> temp = lev_head;
            lev_head = lev_head->right;
        }
    }
    head.reset(new __SkipListNode<T>(T(), 1));
    size_ = 0;
}

/**
 * @brief 获取所有元素
 * 
 * @tparam T 节点保存信息的元素类型
 * @return std::vector<T> 存放的所有元素
 */
template<typename T> std::vector<T> 
SkipList<T>::getElements() {
    std::shared_ptr<__SkipListNode<T>> p = head;
    while (p->down)
        p = p->down;
    if (p)
        p = p->right;
    std::vector<T> ret;
    while (p) {
        ret.push_back(p->value);
        p = p->right;
    }
    return ret;
}

#endif