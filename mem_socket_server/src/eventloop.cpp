#include "eventloop.h"
#include "util.h"
#include "config.h"

/**
 * @brief 构造
 * 
 * @details 初始化：epoll、类型键值对、mysql连接api
 *          相关数据配置在 config.h 内
 * 
 */
EventLoop::EventLoop() :
    ep(new Epoll()),
    memKV({}),
    sqm(new SqlManager()),
    quit(false)
{
    sqm->connect(SQL_IP, SQL_PORT, SQL_USERNAME, SQL_PASSWORD);
    sqm->selectDB(SQL_DBNAME);
}


/**
 * @brief 析构
 * 
 * @details 释放所有的指针
 * 
 */
EventLoop::~EventLoop () {
    if (ep != nullptr) {
        delete ep;
        ep = nullptr;
    }
    if (sqm != nullptr) {
        delete sqm;
        sqm = nullptr;
    }
}

/**
 * @brief Reactor 监听与响应
 * 
 * @details 提出 epoll 树中的就绪事件，执行对应的回调函数
 * 
 */
void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> activityChannels = ep->poll();
        for (auto it = activityChannels.begin(); it != activityChannels.end(); it ++) {
            (*it)->handleEvent();
        }
    }
}


/**
 * @brief 更新套接字集合在 epoll 树中的状态
 * 
 * @param ch  被更新套接字集合
 * @param ope 更新指令（在 epoll::updateChannel 中存在）
 */
void EventLoop::updateChannel(Channel* ch, int ope) {
    ep->updateChannel(ch, ope);
}

/**
 * @brief 操作变量与值的键值对，加锁执行函数 ope
 * 
 * @param ope 被执行函数
 */
void EventLoop::opeMemKV (std::function<void()> ope) {
    mutexMemKV.lock();
    ope();
    mutexMemKV.unlock();
}