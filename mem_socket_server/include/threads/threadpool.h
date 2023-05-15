#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <semaphore.h>
#include <future>
#include <atomic>
#include <functional>


class ThreadsPool {
private:
    std::vector<std::thread> tpool;
  
    std::queue<std::function<void()>> tasks;
    std::mutex mutexTask;
    sem_t semTask;
  
    bool running;

public:
    ThreadsPool ();
    ~ThreadsPool ();

    template<class F, class ...Args>
    auto commit (F &&func, Args &&...args) -> std::future<decltype(func(args...))> {
        using retType = decltype(func(args...));
        if (!running) {
            return std::future<retType>();
        }
      
        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        std::future<retType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mutexTask);
            tasks.emplace([task]{ (*task)(); });
            sem_post(&semTask);
        }
      
        return ret;
    }

    void threadWork ();
};
