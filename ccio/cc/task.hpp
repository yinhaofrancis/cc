#ifndef TASK_HPP
#define TASK_HPP
#include <thread>
#include <functional>
#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>

#include "ref.hpp"
namespace cc
{
    class Pool;
    class Operation
    {
    public:
        Operation(std::function<void()> &action);
        void cancel();
        void exec() const;
        friend class Pool;

    private:
        std::function<void()> m_action;
        std::atomic_bool m_avaiable;
    };

    class Pool
    {
    public:
        Pool(int);
        ~Pool();
        Pool(const Pool&) = delete;
        Pool(const Pool&&) = delete;
        void dispatch(std::function<void()> &&);
        uint64_t count();
        uint64_t workCount();
        void Wait();
    private:
        std::deque<Operation*> m_queue;
        const int m_maxCount;
        void dispatch(Operation*);
        std::mutex m_mutex;
        std::atomic_uint64_t m_count = 0;
        std::atomic_uint64_t m_work_count = 0;
        std::condition_variable m_cv;
        bool is_close = false;
    };
    class Loop{
    public:
        Loop();
        ~Loop();
        Loop(const Loop&) = delete;
        Loop(const Loop&&) = delete;
        void Wait();
        void dispatch(std::function<void()> &&);
    private:
        bool *m_status = nullptr;
        void dispatch(Operation *);
        std::deque<Operation *> m_queue;
        std::mutex m_mutex;
        std::thread* m_thread = nullptr;
    };
} // namespace cc

#endif