#include "../include/channel.h"
#include "../include/threads/threadpool.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cstring>

Channel::Channel () :
    fd(-1),
    loop(nullptr),
    event(0),
    revent(0),
    inEpoll(false),
    name("")
{}

Channel::Channel(EventLoop *_loop, int _fd) :
    fd(_fd),
    loop(_loop),
    event(0),
    revent(0),
    inEpoll(false)
{}

Channel::~Channel() {
    if (~fd){
        close(fd);
        fd = -1;
    }
}

void Channel::handleEvent() {
    loop->threadpool->commit(callback);
    // callback();
}

void Channel::setCallback (std::function<void()> _cb) {
    callback = _cb;
}

void Channel::enableReading() {
    event = EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

void Channel::disableReading() {
    loop->updateChannel(this, -1);
}

int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvent() {
    return event;
}

uint32_t Channel::getRevent() {
    return revent;
}

bool Channel::isInEpoll() {
    return inEpoll;
}
std::string Channel::getName () {
    return name;
}

void Channel::setFd(int _fd) {
    fd = _fd;
}

void Channel::setName (const std::string& _name) {
    name = _name;
}
void Channel::setEvent(uint32_t _event) {
    event = _event;
}

void Channel::setRevent(uint32_t _revent) {
    revent = _revent;
}

void Channel::setInEpoll() {
    inEpoll = true;
}

void Channel::sWrite(const std::string& s) {
    char buf[1024]; bzero(buf, sizeof(buf));
    for (int i = 0; i < s.size(); i ++) {
        buf[i] = s[i];
    }
    buf[s.size()] = '\n';
    if (send(fd, &buf, sizeof(buf), 0) == -1) {
        std::cout << "error: send\n";
    }
}

std::string Channel::nextOrder() {
    std::string ret;
    for (; buf[word_id] != '\n' && buf[word_id] != ' '; word_id ++) {
        if (buf[word_id] != '\0') 
            ret += buf[word_id];
    }
    word_id ++;
    return ret;
}

void Channel::sRead() {
    word_id = 0;
    bzero(buf, sizeof(buf));
    recv(fd, &buf, sizeof(buf), 0);
}