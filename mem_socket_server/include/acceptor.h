#ifndef INCLUDE_ACCEPTOR_H
#define INCLUDE_ACCEPTOR_H

#include "socket.h"
#include "eventloop.h"
#include <functional>

/**
 * @brief 新连接分发器
 * 
 * @details 用于与 mainReactor 监听器搭配，接取新连接后
 *          利用绑定了监听Socket的成员Channel:accept_channel填入回调acceptConnection
 *          accept_channel响应时执行acceptConnection()，也就是成员函数变量 Acceptor::new_connection_callback
 *          Acceptor::new_connection_callback 在 Server 初始化时设置为 Server::handleConnectionEvent()
 */
class Acceptor {
public:
    /* 构造函数，接收 _loop=mainReactor，初始化监听地址 InetAddress 与监听 Socket */
    Acceptor (EventLoop* _loop);

    /* 析构函数，删掉 sock、addr、accept_channel */
    ~Acceptor ();

    /* 接受连接操作：调用 new_connection_callback，作为 accept_channel 的回调函数 */
    void acceptConnection ();

    /* 设置接受新连接时应执行的操作 */
    void setNewConnectionCallback (const std::function<void(Socket*)>& _cb);
private:
    EventLoop*   mainloop;       ///< 搭配的主Reactor
    Socket*      sock;           ///< 监听套接字
    InetAddress* addr;           ///< 监听地址
    Channel*     accept_channel; ///< 绑定了监听套接字的channel
    
    std::function<void(Socket*)> new_connection_callback; ///< 接受新连接时应执行的操作
                                                          ///  在 acceptConnection() 时使用
                                                          ///  作为 accept_channel 的回调
};

#endif