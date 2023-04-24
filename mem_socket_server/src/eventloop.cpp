#include "../include/eventloop.h"
#include "../include/util.h"
#include "../include/config.h"

/**
 * 数据恢复与备份，一个数据的内容为：
 * userName, variableName, variableSize
 * variableByte[0], variableByte[1], ...
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
    restor();
}

EventLoop::~EventLoop () {
    backup();
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
    memKV.clear();
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> activityChannels = ep->poll();
        for (auto it = activityChannels.begin(); it != activityChannels.end(); it ++) {
            (*it)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch, int ope) {
    ep->updateChannel(ch, ope);
}

void EventLoop::backup () {
    freopen("../backup/variable_info.txt", "w", stdout);
    for (const auto& [userName, variableInfo] : memKV) {
        for (const auto& [variableName, ad_sz] : variableInfo) {
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

void EventLoop::opeMemKV (std::function<void()> ope) {
    mutexMemKV.lock();
    ope();
    mutexMemKV.unlock();
}