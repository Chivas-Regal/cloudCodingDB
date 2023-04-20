#pragma once

#include "eventloop.h"
#include "socket.h"

class Server {
public:
    Server (EventLoop *_loop);
    ~Server ();

    void handleConnectionEvent (Socket *serv_sock);
    void handleLoginEvent (Channel* clnt_channel);
    void handleTaskChoose (Channel* clnt_channel);
    // client 申请新建一片空间
    void handleNewMemplace (Channel* clnt_channel);
    // client 申请向某个地址写入数据
    void handleInputValue (Channel* clnt_channel);
    // client 申请向某个地址读出数据
    void handleOutputValue (Channel* clnt_channel);
    // client 申请释放空间
    void handleFreeMemplace (Channel* clnt_channel);
private:
    EventLoop *loop;
};