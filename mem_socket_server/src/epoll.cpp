#include "../include/epoll.h"
#include "../include/util.h"
#include "../include/limits.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

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

std::vector<Channel*> Epoll::poll(int timeout) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif (
        nfds == -1,
        "error epoll poll"
    );
    std::vector<Channel*> activityChannels;
    for (int i = 0; i < nfds; i ++) {
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevent(events[i].events);
        activityChannels.push_back(ch);
    }
    return activityChannels;
}

void Epoll::addFd (int fd, uint32_t op) {
    epoll_event ev; bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif (
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1,
        "error epoll add fd"
    );
}

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

