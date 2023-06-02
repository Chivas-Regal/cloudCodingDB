#include "epoll.h"
#include "util.h"
#include "limits.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

/**
 * @brief 构造函数
 * 
 * @details epoll_create1() 创建 epoll 套接字
 *          创建 epoll_event 数组
 */
Epoll::Epoll () :
    epfd(epoll_create1(0)),
    events(new epoll_event[MAX_EVENTS])
{
    errif (
        epfd == -1,
        "error epoll create"
    );
    bzero(events, sizeof(events));
}

/**
 * @brief 析构函数
 * 
 * @details 关闭 epoll 套接字
 *          释放 events 指针，并设置为 nullptr
 */
Epoll::~Epoll() {
    if (~epfd) {
        close(epfd);
        epfd = -1;
    }
    if (events) {
        delete[] events;
        events = nullptr;
    }
}   

/**
 * @brief 提出 epoll 内的就绪事件的 channel(在 .data.ptr 中)
 * 
 * @details epoll_wait 提出所有事件，然后提出我们在插入或修改 epoll树节点 的时候在 ptr 传入的 channel
 *          将提出的 channel 的当前事件 revent 设置为此事件的就绪事件 events[i].events
 * 
 * @param timeout 用户拿一次数据可以等待的时间
 * @return std::vector<Channel*> 就绪事件的 channel 集
 */
std::vector<Channel*> Epoll::poll(int timeout) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif (
        nfds == -1,
        "error epoll poll"
    );
    std::vector<Channel*> activityChannels;
    for (int i = 0; i < nfds; i ++) {
        Channel *ch = (Channel*)events[i].data.ptr;
        activityChannels.push_back(ch);
    }
    return activityChannels;
}

/**
 * @brief 更新套接字集合在 epoll 树中的状态
 * 
 * @param ch   被更新的套接字集合类
 * @param ope  epoll操作
 *              - -1:   delete
 *              - else: 
 *                  - exit: modify
 *                  - else: addin
 */
void Epoll::updateChannel(Channel* ch, int ope) {
    epoll_event ev; bzero(&ev, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->getEvent();
    if (ope == -1) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, ch->getFd(), &ev);
        return;
    }
    if (ch->isInEpoll()) {
        epoll_ctl(epfd, EPOLL_CTL_MOD, ch->getFd(), &ev);
    } else {
        epoll_ctl(epfd, EPOLL_CTL_ADD, ch->getFd(), &ev);
        ch->setInEpoll();
    }
}

