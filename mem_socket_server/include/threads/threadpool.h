#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <semaphore.h>
#include <future>
#include <atomic>
#include <functional>


/**
 * @brief 线程池
 * 
 * @details 通过 commit 提交任务
 *          以并行的方式同时执行多个任务来提高在高并发场景下得到任务结果的速率
 */
class ThreadsPool {
public:
    /* 创建 cpunumber 个线程，给每个线程设置行为（循环提取任务，执行任务） */
    ThreadsPool ();

    /* running 设置为 false 后等待所有线程关闭 */
    ~ThreadsPool ();

    /**
     * @brief 提交任务
     * 
     * @tparam F    任务函数类型
     * @tparam Args 任务函数的参数类型们
     * @param func  任务函数
     * @param args  任务函数的参数们
     * @return std::future<decltype(func(args...))> 一个可以等待线程池运算出返回值的任务
     */
    template<class F, class ...Args>
    auto commit (F &&func, Args &&...args) -> std::future<decltype(func(args...))> {
        using retType = decltype(func(args...)); // 返回类型
        if (!running) {
            return std::future<retType>();
        }
      

        /* 将 func(args) 绑定在一起，设置为一个 packaged_task* */
        auto task = std::make_shared<std::packaged_task<retType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        std::future<retType> ret = task->get_future();  // 提出 task 中的异步返回值等待类
        /* 将任务 (*task)() 放入任务队列 */
        {
            std::lock_guard<std::mutex> lock(mutexTask);
            tasks.emplace([task]{ (*task)(); });
            sem_post(&semTask);
        }
      
        /* 任务逻辑，获取到 ret = task.future 后，等到
           任务队列中的 (*task)() 执行完毕，ret.get() 可以获得结果 */
        return ret;
    }
private:
    std::vector<std::thread> tpool;             ///< 线程池（多个线程）
    std::queue<std::function<void()>> tasks;    ///< 任务队列
    std::mutex mutexTask;                       ///< 任务队列写锁
    sem_t semTask;                              ///< 是否有任务的信号量
    bool running;                               ///< 是否处于 active
};
