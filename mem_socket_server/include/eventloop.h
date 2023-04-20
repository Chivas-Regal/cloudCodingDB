#pragma once

#include "epoll.h"
#include "sqlmanager.h"
#include "memtools/mempool.h"
#include "threads/threadpool.h"
#include <map>
#include <string>
#include <functional>

class Epoll;
class Channel;

class EventLoop {
public:
    EventLoop ();
    ~EventLoop ();

    void loop ();
    void updateChannel(Channel* ch);

    void backup ();
    void restor ();

    void opeMemKV (std::function<void()> ope);

    /**
     * @key: 变量名称
     * @value: 
     *   - @first:  首地址
     *   - @second: 长度
     */
private:
    Epoll *ep;
    bool quit;

    std::map<
        std::string,        ///< username
        std::map<
            std::string,    ///< variable 
            std::pair<uint8_t*, size_t> ///< {address, size}
        >
    > memKV; 
    std::mutex mutexMemKV;

    SqlManager* sqm;
    MemPool* mempool;
    ThreadsPool* threadpool;
friend class Server;
friend class Channel;
};