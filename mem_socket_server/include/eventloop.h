#pragma once

#include "epoll.h"
#include "sqlmanager.h"
#include "memtools/mempool.h"
#include "threads/threadpool.h"
#include "containers/trie.h"
#include <map>
#include <string>
#include <functional>

class Epoll;
class Channel;

/**
 * @brief Reactor 分发器
 * 
 * @details 用于获取到 this->ep 监听到的就绪事件
 *          然后执行这些就绪事件的回调函数
 *          在整体中存在于一个主分发器以及多个从分发器
 *          每个从分发器位于一个线程池内
 */
class EventLoop {
public:
    /* 初始化：epoll、类型键值对、mysql连接api */
    EventLoop ();

    /* 释放所有的指针 */
    ~EventLoop ();

    /* while1: Reactor 分发 */
    void loop ();

    /* 更新套接字集合在 epoll 树中的状态(被更新套接字，更新指令) 
       指令在 Epoll:updataChannel 中存在说明                 */
    void updateChannel(Channel* ch, int ope = 1);

    /* 按 ope 操作读写 this::memKV */
    void opeMemKV (std::function<void()> ope);
private:
    Epoll *ep;          ///< Reactor 监听的 Epoll 树
    bool quit;          ///< 是否退出监听

    /* {username, {variable_name,  {address, size} }} */
    Trie<Trie<std::pair<uint8_t*, size_t>>> memKV; ///< 字典树存放 [用户名][变量名] : 值
    std::mutex mutexMemKV; ///< 修改字典树需上的锁

    SqlManager* sqm;    ///< 登录用的用户信息表 api
friend class Server;
friend class Channel;
};