#include "eventloop.h"
#include "util.h"
#include "config.h"

/**
 * @brief 构造
 * 
 * @details 初始化：epoll、内存池、类型键值对、mysql连接api、线程池
 *          相关数据配置在 config.h 内
 * 
 */
EventLoop::EventLoop() :
    ep(new Epoll()),
    mempool(new MemPool(MEM_NUM_LISTS, MEM_SIZE_LIST, MEM_POOL_ALGO)),
    memKV({}),
    sqm(new SqlManager()),
    quit(false),
    threadpool(new ThreadsPool())
{
    sqm->connect(SQL_IP, SQL_PORT, SQL_USERNAME, SQL_PASSWORD);
    sqm->selectDB(SQL_DBNAME);
    // restor();
}


/**
 * @brief 析构
 * 
 * @details 释放所有的指针
 * 
 */
EventLoop::~EventLoop () {
    // backup();
    if (ep != nullptr) {
        delete ep;
        ep = nullptr;
    }
    if (sqm != nullptr) {
        delete sqm;
        sqm = nullptr;
    }
    if (mempool != nullptr) {
        delete mempool;
        mempool = nullptr;
    }
    if (threadpool != nullptr) {
        delete threadpool;
        threadpool = nullptr;
    }
}

/**
 * @brief Reactor 分发
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
 * @brief 备份
 * 
 * @details 在 /backup/variable_info.txt 里面做备份
 */
void EventLoop::backup () {
    freopen("../backup/variable_info.txt", "w", stdout);
    for (const auto& [userName, variableInfo] : memKV.getKV()) {
        for (const auto& [variableName, ad_sz] : variableInfo.getKV()) {
            std::cout << userName << " " << variableName << " " << ad_sz.second << std::endl;
            uint8_t *ptr = ad_sz.first;
            for (int i = 0; i < ad_sz.second; i ++) {
                std::cout << *ptr << " ";
                ptr ++;
            }
        }
    }
    freopen("/dev/console", "w", stdout);
}

/**
 * @brief 数据恢复与备份，一个数据的内容为：
 * userName, variableName, variableSize
 * variableByte[0], variableByte[1], ...
*/
void EventLoop::restor () {
    freopen("../backup/variable_info.txt", "r", stdin);
    std::string userName, variableName, size;
    while (std::cin >> userName >> variableName >> size) {
        memKV[userName][variableName].first = (uint8_t*)mempool->allocate(std::stoi(size));
        memKV[userName][variableName].second = std::stoi(size);
        uint8_t* ptr = memKV[userName][variableName].first;
        int siz = std::stoi(size);
        for (int i = 0; i < siz; i ++) {
            int num; std::cin >> num;
            *ptr = num;
            ptr ++;
        }
    }
    freopen("/dev/console", "r", stdin);
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