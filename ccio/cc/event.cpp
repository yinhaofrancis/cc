#include "event.hpp"
#include <iostream>

int cc::EventQueue::change(struct kevent &e) const
{
    return kevent(m_kq, &e, 1, nullptr, 0, 0);
}
int cc::EventQueue::add(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const
{
    return change(ident, filter, EV_ADD, fflag, data);
}
int cc::EventQueue::addOnce(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const
{
    return change(ident, filter, EV_ADD | EV_ONESHOT, fflag, data);
}
int cc::EventQueue::addClear(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const
{
    return change(ident, filter, EV_ADD | EV_CLEAR, fflag, data);
}
int cc::EventQueue::remove(uintptr_t ident, int16_t filter) const
{
    return change(ident, filter, EV_DELETE, 0, 0);
}
int cc::EventQueue::enable(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const
{
    return change(ident, filter, EV_ENABLE, fflag, data);
}
int cc::EventQueue::disable(uintptr_t ident, int16_t filter) const
{
    return change(ident, filter, EV_DISABLE, 0, 0);
}
int cc::EventQueue::change(uintptr_t ident, int16_t filter, uint16_t flag, uint32_t fflag, intptr_t data) const
{
    struct kevent n_change;
    EV_SET(&n_change, ident, filter, flag, fflag, data, nullptr);
    return change(n_change);
}

int cc::EventQueue::add_timer(uint timerId, TimerConfig config, long data) const
{
    return change(timerId,EVFILT_TIMER,EV_ADD,config,data);
}

int cc::EventQueue::add_signal(uint signal, intptr_t data) const
{
    return change(signal,EVFILT_SIGNAL,EV_ADD,0,data);
}

int cc::EventQueue::add_fd_writable(int fd) const
{
    return change(fd,EVFILT_WRITE,EV_ADD | EV_EOF,0,0);
}

int cc::EventQueue::add_fd_readable(int fd,ReadableConfig config) const
{
    return change(fd,EVFILT_READ,EV_ADD | EV_ERROR,config,0);
}

int cc::EventQueue::wait(const struct timespec *timeout,struct kevent& event) const 
{
    return kevent(m_kq,nullptr,0,&event,1,timeout);
}

int cc::EventQueue::wait(const struct timespec *timeout,std::vector<struct kevent>& e) const
{
    memset(m_event->data(),0,m_event->size() * sizeof(struct kevent));
    e.clear();
    int c = kevent(m_kq,nullptr,0,m_event->data(),m_event->size(),timeout);
    for (size_t i = 0; i < c; i++)
    {
        e.push_back((*m_event)[i]);
    }
    return c;
}

int  cc::EventQueue::wait(TimeInterval timeout,std::vector<struct kevent>& e) const{
    int sec = floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nano;
    return wait(&ts,e);
}

int cc::EventQueue::wait(TimeInterval timeout,struct kevent& event) const{
    int sec = floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nano;
    return wait(&ts,event);   
}

int cc::EventQueue::wait(int sec,std::vector<struct kevent>& event) const{
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = 0;
    return wait(&ts,event); 
}

int cc::EventQueue::wait(int sec,struct kevent& event) const{
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = 0;
    return wait(&ts,event); 
}

int cc::EventQueue::add_process(pid_t pid,ProcessConfig config) const{
    return add(pid,EVFILT_PROC,config,0);
}

int cc::EventQueue::add_vnode(int fd,VnodeConfig config) const{
    return addClear(fd,EVFILT_VNODE,config,0);
}