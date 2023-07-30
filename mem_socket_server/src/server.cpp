#include "server.h"
#include "utils.h"
#include "config.h"

#include <string>
#include <cstring>
#include <iostream>

/**
 * @brief 默认构造
 * 
 * @param _loop 主 Reactor 事件分发器件
 * 
 * @details 设置已经建好的主 Reactor 并建立连接提取器 acceptor，将两者搭配起来
 *          将 acceptor 的回调函数设置为连接事件 handleConnectionEvent，这样在 acceptor 内的监听 channel
 *              响应回调的时候会执行 Acceptor::acceptConnection 也就是 acceptor 的回调函数
 *          开 cpunumber 个线程与 cpunumber 个 subReactor ，每个 subReactor 放进一个线程
 */
Server::Server(EventLoop *_loop) :
    mainloop(_loop),
    threadpool(new ThreadsPool()),
    acceptor(nullptr)
{
    acceptor = new Acceptor(mainloop);
    std::function<void(Socket*)> _cb = std::bind(&Server::handleConnectionEvent, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(_cb);

    int cpuNumber = sysconf(_SC_NPROCESSORS_CONF);
    for (int i = 0; i < cpuNumber; i ++) {
        subloops.push_back(new EventLoop());
        threadpool->commit(std::bind(&EventLoop::loop, subloops.back()));
    }
}

/**
 * @brief 析构函数
 * 
 * @details 释放 mainloop、subloops、acceptor、threadpool、mempool
*/
Server::~Server () {
    if (mainloop) {
        delete mainloop;
        mainloop = nullptr;
    }
    for (int i = 0; i < subloops.size(); i ++) {
        delete subloops[i];
    }
    subloops.clear();
    if (acceptor) {
        delete acceptor;
        acceptor = nullptr;
    }
    if (threadpool) {
        delete threadpool;
        threadpool = nullptr;
    }
}

/**
 * @brief 处理连接事件
 * 
 * @param serv_sock 被连接的监听套接字
 * 
 * @details 连接上之后服务端
 *           1 次 send ，内容为 "OK"
 *          将此连接套接字放入 epoll ，在对应的分发器内的回调函数设置为登录事件 handleLoginEvent
 */
void Server::handleConnectionEvent(Socket *serv_sock) {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    std::cout << "new client fd " << clnt_sock->getFd() << std::endl;
    Channel *clnt_channel = new Channel(subloops[clnt_sock->getFd() % subloops.size()], clnt_sock->getFd());
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

    std::cout << "User login: " << username << " " << password << std::endl;

    if (clnt_channel->getLoop()->sqm->QueryGetRes("SELECT * FROM mem_socket_login_infos WHERE username = '" + username + "' and password = '" + password + "';").empty()) { // ERROR: username or password not found
        clnt_channel->disableReading();
        clnt_channel->~Channel();
        std::cout << "error login\n";
        return;
    } else {
        std::cout << "successfully login\n";
    }
    clnt_channel->getLoop()->opeMemKV(
        [&](){
            clnt_channel->getLoop()->memKV[username] = new DB(username);
        }
    );
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
 *              - NEW: 申请新变量空间 handleNewMemplace()
 *              - GET: 获取某个变量的值 handleInputValue()
 *              - SET: 修改某个变量的值 handleOutputValue()
 *              - DES: 回收某个变量空间 handleFreeMemplace()
 *              - QUIT: or default: 关闭连接
 *          下一步就是各自应执行函数，直接调用
 */
void Server::handleTaskChoose(Channel* clnt_channel) {
    clnt_channel->sRead();
    std::string taskId = clnt_channel->nextOrder();
    std::function<void()> _cb;

    std::cout << taskId << "\n";
    if (taskId == "NEW") {
        handleNewMemplace(clnt_channel);
    } else if (taskId == "GET") {
        handleInputValue(clnt_channel);
    } else if (taskId == "SET") {
        handleOutputValue(clnt_channel);
    } else if (taskId == "DES") {
        handleFreeMemplace(clnt_channel);
    } else  { // taskid = 'QUIT' or ERROR: taskid not found
        auto* loop = subloops[clnt_channel->getFd() % subloops.size()];
        loop->memKV[clnt_channel->getName()]->close();
        loop->memKV.erase(clnt_channel->getName());
        clnt_channel->disableReading();
        clnt_channel->~Channel();
        return;
    }
}

/**
 * @brief 申请新变量空间
 * 
 * @param clnt_channel 请求分配的源客户端
 * 
 * @details 从上次在 handleTaskChoose 的 read 中，分离出 2 个参数信息
 *              - name   变量名
 *              - nBytes 变量大小（单位：字节）
 *          打开变量信息表填入信息，并从内存池中申请对应大小的变量
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleNewMemplace(Channel* clnt_channel) {
    std::string name = clnt_channel->nextOrder();
    std::string nBytes = clnt_channel->nextOrder();

    clnt_channel->getLoop()->opeMemKV(
        [&](){
            clnt_channel->getLoop()->memKV[clnt_channel->getName()]->put(name, std::string(std::stoi(nBytes), '0'));
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
 * @details  从上次在 handleTaskChoose 的 read 中，分离出 3 个参数信息
 *              - name    变量名
 *              - _sstart 读取左端点与首地址偏移量
 *              - _send   读取右端点与首地址偏移量
 *          判断本机字节序，看从哪侧拆入 byteBuf[] 中，一字节一字节放进去
 *           1 次 send ，发送长度为约定好的(end - start + 1)的网络字节序字节流
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleInputValue(Channel* clnt_channel) {
    std::string name = clnt_channel->nextOrder();

    auto* loop = subloops[clnt_channel->getFd() % subloops.size()];

    auto [start, len] = loop->memKV[clnt_channel->getName()]->get(name);
    uint8_t byteBuf[len]; bzero(byteBuf, sizeof(byteBuf));
    uint8_t* end = start + len - 1;
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
 * @details  从上次在 handleTaskChoose 的 read 中，分离出 3 个参数信息
 *              - name    变量名
 *              - _sstart 读取左端点与首地址偏移量
 *              - _send   读取右端点与首地址偏移量
 *           1 次 recv ，获取到长度为 (end - start + 1) 的网络字节序的字节流
 *          判断本机字节序，看从哪侧将 byteBuf[] 替换到本地空间中，一字节一字节放进去
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleOutputValue(Channel* clnt_channel) {
    std::string name = clnt_channel->nextOrder();

    auto* loop = subloops[clnt_channel->getFd() % subloops.size()];

    auto [start, len] = loop->memKV[clnt_channel->getName()]->get(name);
    uint8_t* end = start + len - 1;

    uint8_t byteBuf[end - start + 1];
    bzero(byteBuf, sizeof(byteBuf));
    if (recv(clnt_channel->getFd(), byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "recv error\n";
    }

    std::string newv(len, '0');
    if (BitHighLow()) {
        // uint8_t* onemem = start;
        // for (int i = 0; onemem - 1 != end; i ++) {
        //     *onemem = byteBuf[i];
        //     onemem ++;
        // }
        for (int i = 0; i < len; i ++) {
            newv[i] = byteBuf[i];
        }
    } else {
        // uint8_t* onemem = end;
        // for (int i = 0; onemem + 1 != start; i ++) {
        //     *onemem = byteBuf[i];
        //     onemem --;
        // }
        for (int i = 0; i < len; i ++) {
            newv[len - i - 1] = byteBuf[i];
        }
    }

    loop->memKV[clnt_channel->getName()]->put(name, newv);
    
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
 * @details  从上次在 handleTaskChoose 的 read 中，得到一个参数信息 "变量名"
 *          在变量表中将空间释放回内存池，并删除对应变量名
 *           1 次 send ，内容为 "OK"
 *          回调函数修改回处理任务指引事件 handleTaskChoose()
 */
void Server::handleFreeMemplace(Channel* clnt_channel) {
    std::string name = clnt_channel->nextOrder();

    auto* loop = subloops[clnt_channel->getFd() % subloops.size()];

    loop->opeMemKV(
        [&](){
            loop->memKV[clnt_channel->getName()]->erase(name);
        }
    );
    clnt_channel->sWrite("OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}