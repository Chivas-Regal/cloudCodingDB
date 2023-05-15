#include "../../include/threads/threadpool.h"

#include <unistd.h>
#include <iostream>

ThreadsPool::ThreadsPool (): running(true) {
    sem_init(&semTask, false, 0);
    int cpuNumber = sysconf(_SC_NPROCESSORS_CONF);
    std::cout << cpuNumber << std::endl;
    for (int i = 0; i < 1; i ++) {
        tpool.push_back(std::thread(&ThreadsPool::threadWork, this));
    }
}

ThreadsPool::~ThreadsPool() {
    running = false;
    for (int i = 0; i < tpool.size(); i ++) {
        sem_post(&semTask);
    }
    for (std::thread &th : tpool) {
        th.join();
    }
    sem_destroy(&semTask);
}

void ThreadsPool::threadWork() {
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
}
