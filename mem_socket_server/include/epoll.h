#pragma once

#include "channel.h"
#include <vector>
#include <sys/epoll.h>
#include <cstdint>

class Channel;
class EventLoop;

class Epoll {
public:
    Epoll ();
    ~Epoll ();

    std::vector<Channel*> poll (int timeout = -1);

    void addFd (int fd, uint32_t op);
    void updateChannel (Channel* ch);
private:
    int epfd;
    epoll_event *events;
};