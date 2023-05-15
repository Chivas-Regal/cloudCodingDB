#pragma once

#include "eventloop.h"
#include "epoll.h"
#include <functional>
#include <cstdint>
#include <string>

class EventLoop;
class Epoll;

class Channel {
public:
    Channel ();
    Channel (EventLoop *_loop, int _fd);
    ~Channel ();
    void handleEvent ();
    void setCallback (std::function<void()> _cb);
    void enableReading ();
    void disableReading ();

    int getFd ();
    uint32_t getEvent ();
    uint32_t getRevent ();
    std::string getName ();
    bool isInEpoll ();

    void setName (const std::string& _name);
    void setFd (int _fd);
    void setEvent (uint32_t _event);
    void setRevent (uint32_t _revent);
    void setInEpoll ();

    void sWrite (const std::string& s);
    void sRead ();
    std::string nextOrder ();
private:
    int fd;
    std::string name;
    EventLoop *loop;
    uint32_t event;
    uint32_t revent;
    bool inEpoll;
    int word_id;
    std::function<void()> callback;

    char buf[128];
};