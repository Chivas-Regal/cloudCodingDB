#pragma once

#include "eventloop.h"
#include "epoll.h"
#include <functional>
#include <cstdint>
#include <string>

class EventLoop;
class Epoll;

/**
 * @brief socket套接字管理类
 * 
 * @details 基本是一个信息存储类
 *          通过监听自己的 subReactor 管理修改自己在 Epoll 内的状态
 *          可设置就绪状态时执行的回调和执行回调
 *          可做通信简化 this::sRead(), this::nextOrder()
 */
class Channel {
public:
    /* 默认初始化，全部设置为空 */
    Channel ();

    /* 带参构造，绑定监听自己的Reactor分发器和socket套接字 */
    Channel (EventLoop *_loop, int _fd);

    /* 关闭套接字 */
    ~Channel ();

    /* 在本 subReactor 线程内执行事件-回调函数 */
    void handleEvent ();

    /* 设置回调函数 */
    void setCallback (std::function<void()> _cb);

    /* 启用 epoll 内可读 EPOLLIN|EPOLLET */
    void enableReading ();

    /* 关闭 epoll 内可读 */
    void disableReading ();

    /* 获取绑定的套接字 */
    int getFd ();

    /* 获取事件属性 */
    uint32_t getEvent ();

    /* 获取客户端对应的用户名 */
    std::string getName ();

    /* 获取监听本套接字的 subReactor */
    EventLoop* getLoop ();

    /* 获取是否在 epoll 树中 */
    bool isInEpoll ();

    /* 设置用户名 */
    void setName (const std::string& _name);

    /* 设置套接字 */
    void setFd (int _fd);

    /* 设置事件属性 */
    void setEvent (uint32_t _event);

    /* 设置自己在 epoll 的状态为 true */
    void setInEpoll ();

    /* 发送字符串 */
    void sWrite (const std::string& s);

    /* 读指令进 buf[] 中，拆解词组放进 words 中 */
    void sRead ();

    /* 在已读取到的缓冲区 words 中获取下一条语句（不额外读） */
    std::string nextOrder ();
private:
    int fd;             ///< 被绑定套接字
    std::string name;   ///< 套接字对应的客户端的“账号名”
    EventLoop *loop;    ///< 监听本绑定类的 subReactor
    uint32_t event;     ///< 本套接字类的发生事件属性（可读、可写）
    bool inEpoll;       ///< 本套接字类是否在 epoll 树中

    /*! @brief 本套接字类在就绪时应执行的回调函数 */
    std::function<void()> callback;

    std::vector<std::string> words; ///< 读取到的 buf[] 拆解为词组命令
    int word_id;                    ///< 在拆解好的词组命令缓冲区中，要读取走到的位置
};