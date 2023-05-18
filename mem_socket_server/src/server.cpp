#include "server.h"
#include "util.h"
#include "config.h"

#include <string>
#include <cstring>
#include <iostream>

/**
 * @brief 默认构造
 * 
 * @param _loop 主 Reactor 事件分发器件
 * 
 * @details 完成 bind() 与 listen() ，并将监听套接字放入 epoll ，回调函数设置为连接事件 handleConnectionEvent
 */
Server::Server(EventLoop *_loop) :
    loop(_loop)
{
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress(SERVER_IP, SERVER_PORT);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Channel *serv_channel = new Channel(loop, serv_sock->getFd());
    std::function<void()> _cb = std::bind(&Server::handleConnectionEvent, this, serv_sock);
    serv_channel->setCallback(_cb);
    serv_channel->enableReading();
}

Server::~Server () {
    delete loop;
}

/**
 * @brief 处理连接事件
 * 
 * @param serv_sock 被连接的监听套接字
 * 
 * @details 连接上之后服务端
 *           1 次 send ，内容为 "OK"
 *          将此连接套接字放入 epoll ，回调函数设置为登录事件 handleLoginEvent
 */
void Server::handleConnectionEvent(Socket *serv_sock) {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    std::cout << "new client fd " << clnt_sock->getFd() << std::endl;
    Channel *clnt_channel = new Channel(loop, clnt_sock->getFd());
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleLoginEvent, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 处理登录事件
 * 
 * @param clnt_channel 待登录客户端
 * 
 * @details  1 次 recv ，分离出 2 个参数信息 
 *              - username
 *              - password
 *          查询用户表是否存在该用户信息，若不存在直接关闭连接，否则
 *           1 次 send ，内容为 "OK" 
 *          将回调函数修改为处理任务指引事件 handleTaskChoose()
 */
void Server::handleLoginEvent (Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string username = clnt_channel->nextOrder();
    std::string password = clnt_channel->nextOrder();

    std::cout << username << " " << password << std::endl;
    if (loop->sqm->QueryGetRes("SELECT * FROM mem_socket_login_infos WHERE username = '" + username + "' and password = '" + password + "';").empty()) { // ERROR: username or password not found
        clnt_channel->disableReading();
        clnt_channel->~Channel();
        std::cout << "error login\n";
        return;
    } else {
        std::cout << "successfully login\n";
    }
    clnt_channel->setName(username);
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 任务指引事件
 * 
 * @param clnt_channel 待指引客户端
 * 
 * @details  1 次 recv 获取 1 个参数信息
 *              - 任务 id
 *          任务 id:
 *              - 1: 申请新变量空间 handleNewMemplace()
 *              - 2: 获取某个变量的值 handleInputValue()
 *              - 3: 修改某个变量的值 handleOutputValue()
 *              - 4: 回收某个变量空间 handleFreeMemplace()
 *              - default: 关闭连接
 *          将回调函数修改为各自该执行的函数
 */
void Server::handleTaskChoose(Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string taskId = clnt_channel->nextOrder();
    std::function<void()> _cb;
    if (taskId == "1") {
        _cb = std::bind(&Server::handleNewMemplace, this, clnt_channel);
    } else if (taskId == "2") {
        _cb = std::bind(&Server::handleInputValue, this, clnt_channel);
    } else if (taskId == "3") {
        _cb = std::bind(&Server::handleOutputValue, this, clnt_channel);
    } else if (taskId == "4") {
        _cb = std::bind(&Server::handleFreeMemplace, this, clnt_channel);
    } else { // ERROR: taskid not found
        clnt_channel->disableReading();
        clnt_channel->~Channel();
        return;
    }
    // E: 任务分发多了个ack发送
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 申请新变量空间
 * 
 * @param clnt_channel 请求分配的源客户端
 * 
 * @details  1 次 recv ，分离出 2 个参数信息
 *              - name   变量名
 *              - nBytes 变量大小（单位：字节）
 *          打开变量信息表填入信息，并从内存池中申请对应大小的变量
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleNewMemplace(Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string name = clnt_channel->nextOrder();
    std::string nBytes = clnt_channel->nextOrder();
    
    loop->opeMemKV(
        [&](){
            loop->memKV[clnt_channel->getName()][name] = std::make_pair<uint8_t*, size_t>(
                static_cast<uint8_t*>(loop->mempool->allocate(std::stoi(nBytes))),
                static_cast<size_t>(std::stoi(nBytes))
            );
        }
    );

    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 获取某个变量的值
 * 
 * @param clnt_channel 请求获取变量的源客户端
 * 
 * @details  1 次 recv ，分离出 3 个参数信息
 *              - name    变量名
 *              - _sstart 读取左端点与首地址偏移量
 *              - _send   读取右端点与首地址偏移量
 *          判断本机字节序，看从哪侧拆入 byteBuf[] 中，一字节一字节放进去
 *           1 次 send ，发送长度为 128 的网络字节序字节流
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleInputValue(Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string name = clnt_channel->nextOrder();
    std::string _sstart = clnt_channel->nextOrder();
    std::string _send = clnt_channel->nextOrder();
    uint8_t* start = loop->memKV[clnt_channel->getName()][name].first + std::stoi(_sstart);
    uint8_t* end =   loop->memKV[clnt_channel->getName()][name].first + std::stoi(_send);
    uint8_t byteBuf[128]; bzero(byteBuf, sizeof(byteBuf));
    if (BitHighLow()) {
        uint8_t* onemem = start;
        for (int i = 0; true; i ++) {
            byteBuf[i] = *onemem;
            if (onemem == end) break;
            onemem ++;
        }
    } else {
        uint8_t* onemem = end;
        for (int i = 0; true; i ++) {
            byteBuf[i] = *onemem;
            if (onemem == start) break;
            onemem --;
        }
    }
    if (send(clnt_channel->getFd(), byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "send error\n";
    }
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 修改某个变量的值
 * 
 * @param clnt_channel 请求修改变量的源客户端
 * 
 * @details  1 次 recv ，分离出 3 个参数信息
 *              - name    变量名
 *              - _sstart 读取左端点与首地址偏移量
 *              - _send   读取右端点与首地址偏移量
 *           1 次 recv ，获取到长度为 128 的网络字节序的字节流
 *          判断本机字节序，看从哪侧将 byteBuf[] 替换到本地空间中，一字节一字节放进去
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleOutputValue(Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string name = clnt_channel->nextOrder();
    std::string _sstart = clnt_channel->nextOrder();
    std::string _send = clnt_channel->nextOrder();
    uint8_t* start = loop->memKV[clnt_channel->getName()][name].first + std::stoi(_sstart);
    uint8_t* end =   loop->memKV[clnt_channel->getName()][name].first + std::stoi(_send);

    uint8_t byteBuf[128];
    bzero(byteBuf, sizeof(byteBuf));
    if (recv(clnt_channel->getFd(), byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "recv error\n";
    }

    if (BitHighLow()) {
        uint8_t* onemem = start;
        for (int i = 0; onemem - 1 != end; i ++) {
            *onemem = byteBuf[i];
            onemem ++;
        }
    } else {
        uint8_t* onemem = end;
        for (int i = 0; onemem + 1 != start; i ++) {
            *onemem = byteBuf[i];
            onemem --;
        }
    }
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

/**
 * @brief 回收某个变量空间
 * 
 * @param clnt_channel 待回收变量的客户端
 * 
 * @details  1 次 recv ，得到一个参数信息 "变量名"
 *          在变量表中将空间释放回内存池，并删除对应变量名
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleFreeMemplace(Channel* clnt_channel) {
    // loop->mempool->print();
    clnt_channel->sRead();
    std::string name = clnt_channel->nextOrder();
    loop->mempool->deallocate(
        loop->memKV[clnt_channel->getName()][name].first, 
        loop->memKV[clnt_channel->getName()][name].second
    );
    loop->opeMemKV(
        [&](){
            loop->memKV[clnt_channel->getName()].erase(name);
        }
    );
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
    loop->mempool->print();
}