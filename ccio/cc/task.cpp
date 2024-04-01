#include "task.hpp"
#include <iostream>

cc::Operation::Operation(std::function<void()> &action) : m_avaiable(true)
{
    m_action = action;
}

void cc::Operation::cancel()
{
    m_avaiable = false;
}

void cc::Operation::exec() const
{
    if (m_avaiable)
    {
        m_action();
    }
}

cc::Pool::Pool(int maxThread) : m_maxCount(maxThread)
{
}

cc::Pool::~Pool()
{
    is_close = true;
    m_mutex.lock();
    for (auto &&i : m_queue)
    {
        delete i;
    }
    m_mutex.unlock();
    Wait();
}

void cc::Pool::dispatch(Operation *op)
{
    m_mutex.lock();
    m_queue.push_back(op);
    m_mutex.unlock();
    if (m_count == m_work_count && (m_count <= m_maxCount || m_maxCount == 0))
    {
        std::thread([this]()
                    {
                        this->m_count++;
                        while (true)
                        {
                            if(this->is_close){
                                this->m_count--;
                                this->m_cv.notify_one();
                                return;
                            }
                            this->m_mutex.lock();
                            bool limit = this->m_maxCount < this->m_count && this->m_maxCount > 0 || this->m_maxCount == 0;
                            if (this->m_queue.size() == 0 && limit)
                            {
                                this->m_mutex.unlock();
                                break;
                            }
                            if (this->m_queue.size())
                            {
                                auto call = this->m_queue.front();
                                this->m_queue.pop_front();
                                this->m_mutex.unlock();
                                this->m_work_count++;
                                call->exec();
                                this->m_work_count--;
                                delete call;
                            }else{
                                this->m_mutex.unlock();
                                break;
                            }
                        }
                        this->m_count--;
                        this->m_cv.notify_one(); })
            .detach();
    }
}

void cc::Pool::dispatch(std::function<void()> &&call)
{

    dispatch(new Operation(call));
}

uint64_t cc::Pool::count()
{
    return m_count;
}

uint64_t cc::Pool::workCount()
{
    return m_work_count;
}

void cc::Pool::Wait()
{
    std::unique_lock<std::mutex> lck(m_mutex);
    do
    {
        m_cv.wait_for(lck, std::chrono::milliseconds(100));
    } while (m_count > 0);
}

cc::Loop::Loop()
{
    bool status = true;
    this->m_thread = new std::thread([this, status]()
                {
                    this->m_status = const_cast<bool *>(&status);
                    while (status)
                    {
                        m_mutex.lock();
                        size_t size = this->m_queue.size();
                        m_mutex.unlock();
                        while (size > 0)
                        {
                            if(!status){
                                return;
                            }
                            m_mutex.lock();
                            auto front = this->m_queue.front();
                            this->m_queue.pop_front();
                            size = this->m_queue.size();
                            m_mutex.unlock();
                            front->exec();
                            delete front;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                });
    this->m_thread->detach();
}
cc::Loop::~Loop()
{
    m_mutex.lock();
    for (auto i : this->m_queue)
    {
        delete i;
    }
    this->m_queue.clear();
    m_mutex.unlock();
    *(this->m_status) = false;
    // this->m_thread->join();
    delete this->m_thread;
}

void cc::Loop::dispatch(Operation *op)
{
    m_mutex.lock();
    m_queue.push_back(op);
    m_mutex.unlock();
}

void cc::Loop::Wait()
{
    this->m_thread->join();
}

void cc::Loop::dispatch(std::function<void()> &&call)
{
    dispatch(new Operation(call));
}
