#pragma once

#include "channel.h"
#include <vector>
#include <sys/epoll.h>
#include <cstdint>

class Channel;
class EventLoop;

/**
 * @brief epoll 复用封装类
 * 
 * @details 内部有一个 epoll 文件描述符来进行操作
 *          将监听套接字的更新 以及 就绪事件的提取 封装起来
 */
class Epoll {
public:
    /* 构造函数
       创建 epoll 套接字，申请 events[] 使用空间 */
    Epoll ();

    /* 析构函数
       关闭 epoll ，释放 events[] */
    ~Epoll ();

    /* 提出就绪事件(用户拿数据最长等待时间) */
    std::vector<Channel*> poll (int timeout = -1);

    /* 更新套接字集合在 epoll 树中的状态(被更新套接字，更新指令) 
       指令对应操作在定义中 */
    void updateChannel (Channel* ch, int ope = 1);
private:
    int epfd;               ///< 代表一个 epoll 的文件描述符
    epoll_event *events;    ///< 存放就绪事件的集合
};