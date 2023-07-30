#ifndef _INCLUDE_CACHE_H_
#define _INCLUDE_CACHE_H_

#include "trie.h"

template<typename T> struct __lru_node;
template<typename T> struct __lru_list;
template<typename T> struct __lru_list_iterator;

/**
 * @brief 位于 lru 算法内的链表的节点信息
 * 
 * @tparam T 节点含有的值信息
 */
template<typename T>
struct __lru_node {
    using _self         =   __lru_node<T>;
    using value_type    =   T;
    using _iterator     =   __lru_list_iterator<T>; 

    /* 默认初始化 */
    __lru_node () :
            key(""), value(T()),
            prev(nullptr), next(nullptr)
    {}           
    /* 用键值对初始化 */ 
    __lru_node (const std::string& _k, const T& _v) : 
            data({_k, _v, false}),
            key(std::get<0>(data)), value(std::get<1>(data)),
            prev(nullptr), next(nullptr),
            has_modified(std::get<2>(data))
    {}
    /* 用键值对和两个指针初始化 */
    __lru_node (__lru_node<T>* _p, const std::string& _k, const T& _v, __lru_node<T>* _n) :
            data({_k, _v, false}),
            key(std::get<0>(data)), value(std::get<1>(data)),
            prev(_p), next(_n),
            has_modified(std::get<2>(data))
    {}
    ~__lru_node () = default;

    __lru_node<T>* prev; ///< 下一个点
    __lru_node<T>* next; ///< 上一个点
    std::tuple<std::string, T, bool> data; ///< 键值对，是否被修改过
    /* 绑定分开访问操作键和值 */
    std::string& key;     ///< 键（保持为字符串）
    T& value;             ///< 值

    bool& has_modified;    ///< 是否被修改过（决定是否在弹出或者结尾塞入block）

    /* 获取键值对（不支持绑定） */
    std::tuple<std::string, T, bool> getData () {
        return data;
    }
};

/**
 * @brief __lru_list<T> 的迭代器
 */
template<typename T>
struct __lru_list_iterator {
    using   _lru_node_base      =   __lru_node<T>;
    using   _self               =   __lru_list_iterator<T>;
    using   _value_type         =   T;
    using   pointer             =   T*;
    using   const_pointer       =   const T*;
    using   reference           =   T&;
    using   const_reference     =   const T&;

    _lru_node_base* _node; ///< 用来支持移动的内置节点指针

    /* 使用已有指针初始化或是new一个节点 */
    explicit __lru_list_iterator () : _node(new _lru_node_base()) {}
    explicit __lru_list_iterator (_lru_node_base* p) : _node(p)   {}
    ~__lru_list_iterator() = default;

    /* 判断指针是否相等 */
    bool operator == (const _self& that) { return _node == that._node; }
    bool operator != (const _self& that) { return _node != that._node; }

    /* 赋值构造 */
    _self& operator = (const _self& that) { *this = that; return *this; }
    /* 指针运算符（不支持直接修改） */
    std::tuple<std::string, T, bool> operator * () { return _node->getData(); }
    _lru_node_base* operator -> () { return _node; }
    /* 迭代器在链表上移动的运算 */
    _self& operator ++ () { _node = _node->next; return *this; }
    _self& operator -- () { _node = _node->prev; return *this; }
    _self& operator ++ (int) { _self _temp = *this; _node = _node->next; return _temp; }
    _self& operator -- (int) { _self _temp = *this; _node = _node->prev; return _temp; }
};

/**
 * @brief 每个节点均存储 key=std::string, value=T 的链表
 * 
 * 位于 LRUCache 内的核心数据结构，与 LRU 算法相配合使用
 * 支持迭代器、头插，删除 ...
 */
template<typename T>
struct __lru_list {
    using value_type        =   T;
    using pointer           =   T*;
    using iterator          =   __lru_list_iterator<T>;
    using _list_node_base   =   __lru_node<T>;

    /* 初始化 head <-> tail */
    __lru_list () : size_(0) {
        head = new __lru_node<T>("", T());
        tail = new __lru_node<T>(head, "", T(), nullptr);
        head->next = tail;
    }

    /* 返回存放的键值对的数量 */
    int size () { return size_; }

    /* 返回最后一个保存键值对的节点（没有则为nullptr） */
    __lru_node<T>* back () { return head->next == tail ? nullptr : tail->prev; }

    /* 返回第一个保存键值对的节点（没有则为nullptr） */
    __lru_node<T>* front () { return head->next == tail ? nullptr : head->next; }
    
    /* 返回第一个保存键值对的迭代器 */
    iterator begin () { return iterator(head->next); }

    /* 返回 tail 的迭代器 */
    iterator end () { return iterator(tail); }

    /* 将 (*)node 从链表中弹出，不释放空间 */
    void erase_without_delete (__lru_node<T>* node) {
        size_ --;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    /* 将 (*)node 从链表中弹出，并释放空间 */
    void erase_with_delete (__lru_node<T>* node) {
        erase_without_delete(node);
        delete node;
        node = nullptr;
    }

    /* 头插入新节点 (*)node */
    void push_front (__lru_node<T>* node) {
        size_ ++;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
        node->prev = head;
    }

    /* 将 (*)node 从原先位置移动到链首 */
    void move_front (__lru_node<T>* node) {
        erase_without_delete(node);
        push_front(node);
    }

    /* 链首链尾节点，不储存信息，只做哨兵 */
    __lru_node<T> *head, *tail;
    int size_; ///< 链表存储信息节点的数量
};

/**
 * @brief 搭载 LRU 算法的缓存器（非冷热部）
 * 
 * 以 put()、get() 为主要功能部分，对键值对进行插入与查询
 * 并在这类访问操作下完成对键值对的置顶
 * 
 * 通过调用内置键值对链表 __lru_list 的迭代器与 begin(), end()
 * 实现出一个可供使用的伪迭代器
 * 
 * @tparam std::string 键的类型
 * @tparam T 值的类型
 * 
 */
template<typename T>
class LRUCache {
public:
    using value_type        =   T;
    using pointer           =   T*;
    using iterator          =   __lru_list_iterator<T>;
    using _list_node_base   =   __lru_node<T>;

    LRUCache(int capacity) : capacity_(capacity), lrulist(__lru_list<T>()) {}

    /**
     * @brief 析构操作
     * 
     * 释放 lrulist 的每一个节点并置空
     */
    ~LRUCache () {
        while (lrulist.back()) {
            lrulist.erase_with_delete(lrulist.back());
        }
        delete lrulist.head;
        delete lrulist.tail;
        lrulist.head = lrulist.tail = nullptr;
    }
    
    /**
     * @brief 通过 key 获取缓存内的 value
     * 
     * @param key 待获取 value 的 key
     * @return T 获取 value 的类型
     */
    T get(const std::string& key) {
        if (!trie.count(key)) 
            return T();
        lrulist.move_front(trie[key]);
        return trie[key]->value;
    }
    
    /**
     * @brief 向缓存内塞入 {key, value}
     * 
     * @param key 插入键
     * @param value 插入值
     * @param new_kv 插入的是否是用户新设定的 kv
     * 
     * 优化于，上层调用是 DBImpl 检测有无弹出，弹出的写到 ssttable 里面
     * 若全都是 get 出来没有修改的那不应该写入 ssttable，故不弹出
     * 所以接收一个标记是否为用户手动新添或修改的信息，在节点插入时用这个信息去标记节点的 has_modified
     * 
     * * 有弹出但是没有修改的话，只有 key 是 "" ，value 不为空
     * 
     * @return std::pair<std::string, T> 
     *      - {"", ...}: 没有弹出
     *      - else:     : 返回 lrulist 弹出的键值对
     */
    std::pair<std::string, T> put(const std::string& key, const T& value, bool new_kv = true) {
        std::pair<std::string, T> ret = {"", T()};
        if (trie.count(key)) {
            trie[key]->value = value;
            trie[key]->has_modified = new_kv;
            lrulist.move_front(trie[key]);
            return ret;
        }
        if (lrulist.size() == capacity_) {
            __lru_node<T>* back = lrulist.back();
            trie.erase(back->key);
            ret.second = back->value;
            if (back->has_modified)
                ret = {back->key, back->value};
            lrulist.erase_with_delete(back);
        }
        trie[key] = new __lru_node<T>(key, value);
        lrulist.push_front(trie[key]);
        trie[key]->has_modified = new_kv;
        return ret;
    }

    /* 获取缓存内键值对的数量 */
    size_t size () {
        return lrulist.size();
    }

    /* 获取缓存链表内的首迭代器 */
    iterator begin () { return lrulist.begin(); }
    /* 获取缓存链表内的尾哨兵迭代器 */
    iterator end () { return lrulist.end(); }

private:
    __lru_list<T> lrulist;      ///< 存储用户键值对的链表，按保留优先级顺序存放
    int capacity_;              ///< LRU 存储容量

    /**
     * key   =  LRU.list.key
     * value =  (*)LRU.list.node
     * 可以通过 key 快速定位链表节点，对其进行移动
     */
    Trie<__lru_node<T>*> trie;
};

#endif
