#include "acceptor.h"
#include "config.h"

/**
 * @brief 构造函数
 * 
 * @param _loop mainReactor
 * 
 * @details 初始化监听地址 InetAddress 与监听 Socket
 *          将 accept_channel 绑定监听 Socket 并将回调设置为 acceptConnection()
 */
Acceptor::Acceptor(EventLoop* _loop) :
        mainloop(_loop),
        sock(new Socket()),
        addr(new InetAddress(SERVER_IP, SERVER_PORT)),
        accept_channel(new Channel(mainloop, sock->getFd()))
{   
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();

    std::function<void()> cb_acceptchannel = std::bind(&Acceptor::acceptConnection, this);
    accept_channel->setCallback(cb_acceptchannel);
    accept_channel->enableReading();
}

/**
 * @brief 析构函数
 * 
 * @details 删掉 sock、addr、accept_channel
 */
Acceptor::~Acceptor() {
    if (sock) {
        delete sock;
        sock = nullptr;
    }
    if (addr) {
        delete addr;
        addr = nullptr;
    }
    if (accept_channel) {
        delete accept_channel;
        accept_channel = nullptr;
    }
}

/**
 * @brief 接受新连接操作
 * 
 * @details 调用 new_connection_callback，作为 accept_channel 的回调函数
 */
void Acceptor::acceptConnection() {
    new_connection_callback(sock);
}

/**
 * @brief 设置接受新连接时应执行的操作
 * 
 * @param _cb 接受新连接时应执行的操作
 */
void Acceptor::setNewConnectionCallback(const std::function<void(Socket*)>& _cb) {
    new_connection_callback = _cb;
}