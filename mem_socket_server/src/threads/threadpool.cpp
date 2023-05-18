#include "../../include/threads/threadpool.h"

#include <unistd.h>
#include <iostream>

/**
 * @brief 构造函数
 * 
 * @details 创建 cpu_number 个线程，每个线程里面设置函数
 *          “每次循环等待从任务队列里面提出一个任务（用信号量标记任务的存在性），任何执行它”
 */
ThreadsPool::ThreadsPool (): running(true) {
    sem_init(&semTask, false, 0);
    int cpuNumber = sysconf(_SC_NPROCESSORS_CONF);
    std::cout << "Threads number: " << cpuNumber << std::endl;
    for (int i = 0; i < 1; i ++) {
        tpool.push_back(std::thread([&]() {
            while (true) {
                sem_wait(&semTask);
                if (!running) break;
                std::function<void()> task;
                {
                    std::lock_guard<std::mutex> lock(mutexTask);
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

/**
 * @brief 析构函数
 * 
 * @details running 设为关闭，然后等待所有线程结束
 */
ThreadsPool::~ThreadsPool() {
    running = false;
    // 这样可以支持所有线程通过 sem_wait(&semTask); 这一步
    for (int i = 0; i < tpool.size(); i ++) {
        sem_post(&semTask);
    }
    for (std::thread &th : tpool) {
        th.join();
    }
    sem_destroy(&semTask);
}
