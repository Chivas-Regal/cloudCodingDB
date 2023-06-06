#include "channel.h"
#include "threads/threadpool.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <string>

/**
 * @brief 默认初始化，全部设置为空
 */
Channel::Channel () :
    fd(-1),
    loop(nullptr),
    event(0),
    inEpoll(false),
    name("")
{}

/**
 * @brief 带参构造
 * 
 * @param _loop 本套接字绑定类的 Reactor分发器
 * @param _fd   本套接字绑定类的套接字
 */
Channel::Channel(EventLoop *_loop, int _fd) :
    fd(_fd),
    loop(_loop),
    event(0),
    inEpoll(false)
{}

/**
 * @brief 析构，关闭套接字
 */
Channel::~Channel() {
    if (~fd){
        close(fd);
        fd = -1;
    }
}

/**
 * @brief 执行事件
 * 
 * @details 执行回调函数（已经在subReactor线程池内部了）
 */
void Channel::handleEvent() {
    callback();
}

/**
 * @brief 设置回调函数
 * 
 * @param _cb 应成为的回调函数
 */
void Channel::setCallback (std::function<void()> _cb) {
    callback = _cb;
}

/**
 * @brief 启用 epoll 内可读 EPOLLIN|EPOLLET
 * 
 * @details 启用完之后再 eventloop 内更新此套接字绑定类
 * 
 */
void Channel::enableReading() {
    event = EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

/**
 * @brief 关闭 epoll 内可读
 * 
 * @details 在 eventloop 中将自己删除掉
 */
void Channel::disableReading() {
    loop->updateChannel(this, -1);
}

/**
 * @brief 获取本绑定的套接字
 */
int Channel::getFd() {
    return fd;
}

/**
 * @brief 获取事件属性
 */
uint32_t Channel::getEvent() {
    return event;
}

/**
 * @brief 获取是否在 epoll 树中
 */
bool Channel::isInEpoll() {
    return inEpoll;
}

/**
 * @brief 获取客户端对应的用户名
 */
std::string Channel::getName () {
    return name;
}

/**
 * @brief 获取监听本套接字的 subReactor
*/
EventLoop* Channel::getLoop() {
    return loop;
}

/**
 * @brief 设置套接字
 * 
 * @param 设置后的套接字
 */
void Channel::setFd(int _fd) {
    fd = _fd;
}

/**
 * @brief 设置用户名
 * 
 * @param _name 设置后的用户名
 */
void Channel::setName (const std::string& _name) {
    name = _name;
}

/**
 * @brief 设置事件属性
 * 
 * @param _event 设置后的事件属性
 */
void Channel::setEvent(uint32_t _event) {
    event = _event;
}

/**
 * @brief 设置自己在 epoll 的状态为 true
 */
void Channel::setInEpoll() {
    inEpoll = true;
}

/**
 * @brief 发送字符串
 * 
 * @param s 待发送字符串
 */
void Channel::sWrite(const std::string& s) {
    char buf[128]; bzero(buf, sizeof(buf));
    for (int i = 0; i < s.size(); i ++) {
        buf[i] = s[i];
    }
    buf[s.size()] = '\n';
    if (send(fd, buf, sizeof(buf), 0) == -1) {
        std::cout << "error: send\n";
    }
}

/**
 * @brief 在已读取到的缓冲区 words 中获取下一条语句（不额外读）
 * 
 * @return std::string 返回下一条命令
 */
std::string Channel::nextOrder() {
    return words[word_id ++];
}

/**
 * @brief 读指令
 * 
 * @details 读进 buf[] 中，拆解词组放进 words 中
 */
void Channel::sRead() {
    word_id = 0; words.clear();
    char buf[128]; bzero(buf, sizeof(buf));
    recv(fd, buf, sizeof(buf), 0);

    std::string s;
    int idx = 0;
    
    s.clear();
    while (buf[idx] != '#') idx ++; idx ++;
    while (buf[idx] != '\n' || buf[idx - 1] != '\r') s += buf[idx ++];
    s.pop_back();
    int n = std::stoi(s);

    for (int i = 0; i < n; i ++) {
        s.clear();
        while (buf[idx] != '$') idx ++; idx ++;
        while (buf[idx] != ' ') s += buf[idx ++];
        int nn = std::stoi(s);
        while (buf[idx] == ' ') idx ++;
        s.clear();
        while (buf[idx] != '\n' || buf[idx - 1] != '\r') s += buf[idx ++];
        s.pop_back();
        words.push_back(s);
    }

    for (auto it : words) std::cout << it << " "; std::cout << "\n";
}