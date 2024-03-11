#include "task.hpp"

cc::WorkFlow::WorkFlow()
{
    std::queue<std::function<void()>> *c_queue = new std::queue<std::function<void()>>();
    std::mutex *c_mutex = new std::mutex();
    std::atomic_bool *c_running = new std::atomic_bool(true);
    this->m_queue = c_queue;
    this->m_mutex = c_mutex;
    this->m_running = c_running;
    auto runner = [c_running, c_mutex, c_queue]()
    {
        while (c_running)
        {
            int size = 0;
            do
            {
                c_mutex->lock();
                size = c_queue->size();
                c_mutex->unlock();
                if (size > 0)
                {
                    c_mutex->lock();
                    auto call = c_queue->front();
                    c_queue->pop();
                    c_mutex->unlock();
                    call();
                }
            } while (size > 0);
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        delete c_queue;
        delete c_mutex;
        delete c_running;
    };
    std::thread(runner).detach();
}
cc::WorkFlow::~WorkFlow()
{
    (*this->m_running) = false;
}
void cc::WorkFlow::async(std::function<void()> func)
{
    this->m_mutex->lock();
    this->m_queue->push(func);
    this->m_mutex->unlock();
}
void cc::WorkFlow::sync(std::function<void()> func)
{
    std::mutex *m = new std::mutex();
    m->lock();
    this->m_mutex->lock();
    this->m_queue->push([m,func](){
        func();
        m->unlock();
        delete m;
    });
    this->m_mutex->unlock();
    m->lock();
}

cc::Task::Task(std::function<void()> func):m_func(func){}

void cc::Task::detach()
{
    std::thread(m_func).detach();
}
void cc::Task::join()
{
    std::thread(m_func).join();
}

void cc::Task::async(std::function<void()> func){
    cc::Task(func).detach();
}

void cc::Task::sync(std::function<void()> func){
    cc::Task(func).join();
}