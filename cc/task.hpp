#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
namespace cc
{
    class WorkFlow
    {
    public:
        WorkFlow();
        ~WorkFlow();
        void async(std::function<void()> func);
        void sync(std::function<void()> func);
    private:
        std::atomic_bool *m_running;
        std::queue<std::function<void()>> *m_queue;
        std::mutex *m_mutex;
    };

    class Task
    {
    public:
        Task(std::function<void()> func);
        ~Task();
        void detach();
        void join();
        static void async(std::function<void()> func);
        static void sync(std::function<void()> func);
    private:
        std::function<void()> m_func;
        std::thread *m_thread;
    };
} // namespace cc

#endif