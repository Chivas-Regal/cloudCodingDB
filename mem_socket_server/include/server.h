#pragma once

#include "eventloop.h"
#include "socket.h"
#include "util.h"
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
    /* 默认构造，需绑定主 Reactor 分发器 */
    Server (EventLoop *_loop);

    /* 析构，和 loop 一起释放 */
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

    EventLoop *loop; ///< 主监听 Reactor 分发器
};