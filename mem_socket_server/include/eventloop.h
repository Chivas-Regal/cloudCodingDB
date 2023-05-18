#pragma once

#include "epoll.h"
#include "sqlmanager.h"
#include "memtools/mempool.h"
#include "threads/threadpool.h"
#include <map>
#include <string>
#include <functional>

class Epoll;
class Channel;

/**
 * @brief Reactor 主分发器
 * 
 * @details 用于获取到 this->ep 监听到的就绪事件
 *          然后执行这些就绪事件的回调函数（分发进线程池）
 */
class EventLoop {
public:
    /* 初始化：epoll、内存池、类型键值对、mysql连接api、线程池 */
    EventLoop ();

    /* 释放所有的指针 */
    ~EventLoop ();

    /* while1: Reactor 分发 */
    void loop ();

    /* 更新套接字集合在 epoll 树中的状态(被更新套接字，更新指令) 
       指令在 Epoll:updataChannel 中存在说明                 */
    void updateChannel(Channel* ch, int ope = 1);

    /* 备份 */
    void backup ();

    /* 恢复 */
    void restor ();

    /* 按 ope 操作读写 this::memKV */
    void opeMemKV (std::function<void()> ope);
private:
    Epoll *ep;          ///< Reactor 监听的 Epoll 树
    bool quit;          ///< 是否退出监听

    std::map<
        std::string,  ///< username
        std::map<
            std::string,                ///< variable_name 
            std::pair<uint8_t*, size_t> ///< {address, size}
        >
    > memKV; 
    std::mutex mutexMemKV;

    SqlManager* sqm;    ///< 登录用的用户信息表 api
    MemPool* mempool;   ///< 分配变量内存的内存池
    ThreadsPool* threadpool; ///< 并行处理事件的线程池
friend class Server;
friend class Channel;
};