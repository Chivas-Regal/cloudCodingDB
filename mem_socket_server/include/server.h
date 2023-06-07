#pragma once

#include "eventloop.h"
#include "socket.h"
#include "util.h"
#include "acceptor.h"
#include <cstring>

/**
 * @brief 服务端任务处理类
 * 
 * @details 组织不同套接字管理类的
 *              - 连接事件
 *              - 登录事件
 *              - 申请...事件
 *          以及它们对应的应做的下一个事件
 *          
 *          !!!!是本工程功能实现的核心逻辑设计!!!!
 */
class Server {
public:
    /* 默认构造，需绑定主 Reactor 分发器，内部构建出多个 subReactor 并放入线程池 */
    Server (EventLoop *_loop);

    /* 析构，和 mainloop、subloops、acceptor、threadpool、mempool 一起释放 */
    ~Server ();

    /* 处理客户端连接事件(监听套接字) */
    void handleConnectionEvent (Socket *serv_sock);

    /* 处理客户端登录事件(用户套接字绑定类) */
    void handleLoginEvent (Channel* clnt_channel);

    /* 处理客户端指令选择事件(用户套接字绑定类) */
    void handleTaskChoose (Channel* clnt_channel);

    /* 处理客户端请求申请新变量空间(用户套接字绑定类) */
    void handleNewMemplace (Channel* clnt_channel);

    /* 处理客户端请求获取某个变量值(用户套接字绑定类) */
    void handleInputValue (Channel* clnt_channel);

    /* 处理客户端请求修改某个变量值(用户套接字绑定类) */
    void handleOutputValue (Channel* clnt_channel);

    /* 处理客户端请求回收某个变量空间(用户套接字绑定类)*/
    void handleFreeMemplace (Channel* clnt_channel);
private:
    Acceptor* acceptor;                 ///< 主 Reactor 搭配的连接提取器
    EventLoop *mainloop;                ///< 主监听 Reactor 分发器
    std::vector<EventLoop*> subloops;   ///< 从服务 Reactor 分发器
    ThreadsPool* threadpool;            ///< 并行处理事件的线程池
    MemPool* mempool;                   ///< 分配变量内存的内存池
};