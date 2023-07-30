#ifndef INCLUDE_TRIE_TRIE_H
#define INCLUDE_TRIE_TRIE_H

#include <string>
#include <functional>
#include <vector>
#include <iostream>

/************ 字典树的字符与整形的双射 ************/
inline int ctoi (char c) {
    if (std::islower(c)) return c - 'a';
    if (std::isupper(c)) return c - 'A' + 26;
    if (std::isdigit(c)) return c - '0' + 52;
    return 62;
}
inline char itoc (int i) {
    if (0 <= i && i < 26) return 'a' + i;
    i -= 26;
    if (0 <= i && i < 26) return 'A' + i;
    i -= 26;
    if (0 <= i && i < 10) return '0' + i;
    i -= 10;
    return '_';
}   
/************************************************/

/**
 * @brief 字典树
 * 
 * @tparam T value类型(key默认为string)
 * 
 * @details 时间复杂度
 *          - 查询 O(size)
 *          - 插入 O(size)
 *          - 遍历 O(sum_size)
 *          空间复杂度最坏情况为 sum_size 个节点
 */
template<class T> class
Trie {
public:
    /* 字典树构造初始化：新建根节点 */
	Trie();

    /* 把 root delete(delete内有递归效果)掉 */
	~Trie();

    /* 在字典树上路径为s的终点处存入value */
	void insert(const std::string& s, T value);

    /* 查询 key=s 对应的 value 指针 */
	T*   query (const std::string& s);

    /* key值是否存在 */
    bool count (const std::string& s);

    /* 删除一个key=s的键值对 */
    void erase (const std::string& s);

    /* 清空字典树 */
    void clear ();

    /* 打印出所有的 key（不管是否存在 value） */
    void dfs ();

    /* 获取字典树内键值对的数量 */
    size_t size ();

    /* 做值为引用的键值对（遍历支持类似于 map 的结构化绑定） */
    std::vector<std::pair<std::string, T&>> getKV ();

    /* 重载 [] ，可以用字符串当下标操作值 */
    T& operator [](const std::string& s);
private:


    /**
     * @brief 字典树节点
     */
	struct TrieNode {
		TrieNode**   child; ///< 儿子，默认有 26+26+10+1=63 个
		T*           value; ///< 节点指向的对应类型的值
        int          size;  ///< 以本节点为根的子树中值的数量

        /* 创建有 63 个元素的 child 指针数组，别的都初始化为 0 */
		TrieNode();

        /* 析构本节点与以本节点为根子树所有节点 */
        ~TrieNode ();
	} *root; ///< 根节点
};

/**
 * @brief 节点构造初始化
 * 
 * @tparam T value参数
 * 
 * @details 创建有 63 个元素的 child 指针数组，别的都初始化为 0
 */
template <class T>
Trie<T>::TrieNode::TrieNode() : child(new TrieNode*[63]), value(nullptr), size(0) {
    for (int i = 0; i < 63; i ++)
        child[i] = nullptr;
}

/**
 * @brief 节点析构
 * 
 * @tparam T value类型
 * 
 * @details 采用类似于递归的连锁 delete
 *          如果哈希第 i 个桶有儿子，就递归地去 delete 这个儿子
 *          最后要把儿子数组、权值删掉（删之前要判断是否为空，删之后要设置为空）
 */
template <class T>
Trie<T>::TrieNode::~TrieNode() {
    if (child) {
        for (int i = 0; i < 63; i ++) {
            if (child[i]) {
                delete child[i];
                child[i] = nullptr;
            }
        }
        delete[] child;
        child = nullptr;
    }
    value = nullptr;
}

/**
 * @brief 字典树构造初始化
 * 
 * @tparam T value类型
 * 
 * @details 新建根节点
 */
template<class T>
Trie<T>::Trie() {
	root = new TrieNode();
}

/**
 * @brief 把 root delete(delete内有递归效果)掉
 * 
 * @tparam T value类型
 */
template<class T>
Trie<T>::~Trie() {
    delete root;
    root = nullptr;
}

/**
 * @brief 在字典树上路径为s的终点处存入value
 * 
 * @tparam T        value类型
 * @param s         路径字符串（字典树key）
 * @param _value    待存value
 * 
 * @details 向下走，没有路径就新建一条，然后沿着字符串顺序字符向下走
 *          走到最后根据当前 value 值是否已经初始化来决定怎么赋值
 *          注意新建 value 的话要在路上更新一路 size
 */
template<class T>
void Trie<T>::insert(const std::string &s, T _value) {
    std::vector<TrieNode*> path = {root};
	TrieNode* p = root;
	for (char c : s) {
		if (!p->child[ctoi(c)]) {
			p->child[ctoi(c)] = new TrieNode();
        }
		p = p->child[ctoi(c)];
        path.push_back(p);
	}
	if (!p->value) {
        p->value = new T(_value);
        for (int i = 0; i < path.size(); i ++)
            path[i]->size ++;
    } else
        *(p->value) = _value;
}

/**
 * @brief 查询 key=s 对应的 value 指针
 * 
 * @tparam T value参数
 * @param s  key字符串
 * @return T 
 *          - not null: key字符串对应的value指针
 *          - null:     没有这样的key
 */
template<class T>
T* Trie<T>::query(const std::string &s) {
	TrieNode* p = root;
	for (char c : s) {
		if (!p->child[ctoi(c)])
			return nullptr;
        p = p->child[ctoi(c)];
	}
	return p->value;
}

/**
 * @brief 重载 [] ，可以用字符串当下标操作值
 * 
 * @tparam T   value类型
 * @param s    下标字符串，也是 key值
 * @return T&  引用的value值，可以操作
 *
 * @details 和 insert(s) 是很类似的，如果新建 value 了就把这条路 size 自加一下
 */
template <class T> T&
Trie<T>::operator[](const std::string &s) {
    std::vector<TrieNode*> path;
    TrieNode* p = root;
	for (char c : s) {
        path.push_back(p);
		if (!p->child[ctoi(c)]) {
			p->child[ctoi(c)] = new TrieNode();
        }
		p = p->child[ctoi(c)];
	}
    path.push_back(p);
    if (!p->value) {
        p->value = new T();
        for (int i = 0; i < path.size(); i ++) {
            path[i]->size ++;
        }
    }
    return *(p->value);
}

/**
 * @brief key值是否存在
 * 
 * @tparam T value类型
 * @param s 查询的 key 值
 * @return true 存在
 * @return false 不存在
 */
template <class T> bool
Trie<T>::count (const std::string& s) {
    return query(s) != nullptr;
}

/**
 * @brief 删除一个key=s的键值对
 * 
 * @tparam T value类型
 * @param s  被删除键值对的键
 * 
 * @details 要修改size，同时还要将无用纸条删掉防止浪费空间和 getKV 的时间
 *          若存在无用部分说明 trie[s] 下面没有节点
 *          然后向上找到第一个还有别的儿子的节点或者 root ，把子树 delete 掉
 */
template <class T> void
Trie<T>::erase (const std::string& s) {
    std::vector<TrieNode*> path = {root};
    TrieNode* p = root;
    for (char c : s) {
        if (!p->child[ctoi(c)])
            return;
        p = p->child[ctoi(c)];
        path.push_back(p);
    }
    for (int i = 0; i < path.size(); i ++) 
        path[i]->size --;
    p->value = nullptr;
    if (p->size) return;
    for (int i = path.size() - 1; i >= 0; i --) {
        if (path[i]->size) { // 找到第一个有别的儿子的
            delete path[i]->child[ctoi(s[i])];
            path[i]->child[ctoi(s[i])] = nullptr;
            return;
        }
    }
    delete path[0]->child[ctoi(s[0])]; // 都没有，根节点下面节点删了
    path[0]->child[ctoi(s[0])] = nullptr;
}

/**
 * @brief 清空字典树
 * 
 * @tparam T 值类型
 */
template<typename T> void 
Trie<T>::clear() {
    delete root;
    root = new TrieNode();
}

/**
 * @brief 打印出所有的 key（不管是否存在 value）
 * 
 * @tparam T value类型
 */
template <class T> void 
Trie<T>::dfs() {
    std::string path;
    std::function<void(TrieNode*)> dfs = [&](TrieNode* p) {
        std::cout << path << "\n";
        for (int i = 0; i < 63; i ++) {
            if (p->child[i]) {
                path += i + 'a';
                dfs(p->child[i]);
                path.pop_back();
            }
        }
    };
    dfs(root);
}

/**
 * @brief 返回字典树中键值对数量
 * 
 * @tparam T 值类型
 * @return size_t 字典树中键值对数量
 */
template<class T> size_t
Trie<T>::size () {
    return root->size;
}

/**
 * @brief 做值为引用的键值对（遍历支持类似于 map 的结构化绑定）
 * 
 * @tparam T value类型
 * @return std::vector<std::pair<std::string, T &>> 一个集合，内部键值对中键为常量访问，值为引用访问，支持操作
 */
template <class T> std::vector<std::pair<std::string, T &>> 
Trie<T>::getKV() {
    std::vector<std::pair<std::string, T &>> ret;
    std::string path;
    std::function<void(TrieNode*)> dfs = [&](TrieNode* p) {
        if (p->value)
            ret.push_back({path, *(p->value)});
        for (int i = 0; i < 63; i ++) {
            if (p->child[i]) {
                path += itoc(i);
                dfs(p->child[i]);
                path.pop_back();
            }
        }
    };
    dfs(root);
    return ret;
}
#endif