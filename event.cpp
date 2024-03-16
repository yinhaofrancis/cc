#include "event.hpp"
#include <iostream>

void cc::EventQueue::change(struct kevent &e)
{
    kevent(m_kq, &e, 1, nullptr, 0, 0);
}
void cc::EventQueue::add(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data)
{
    change(ident, filter, EV_ADD, fflag, data);
}
void cc::EventQueue::addOnce(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data)
{
    change(ident, filter, EV_ADD | EV_ONESHOT, fflag, data);
}
void cc::EventQueue::addClear(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data)
{
    change(ident, filter, EV_ADD | EV_CLEAR, fflag, data);
}
void cc::EventQueue::remove(uintptr_t ident, int16_t filter)
{
    change(ident, filter, EV_DELETE, 0, 0);
}
void cc::EventQueue::enable(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data)
{
    change(ident, filter, EV_ENABLE, fflag, data);
}
void cc::EventQueue::disable(uintptr_t ident, int16_t filter)
{
    change(ident, filter, EV_DISABLE, 0, 0);
}
void cc::EventQueue::change(uintptr_t ident, int16_t filter, uint16_t flag, uint32_t fflag, intptr_t data)
{
    struct kevent n_change;
    EV_SET(&n_change, ident, filter, flag, fflag, data, nullptr);
    change(n_change);
}

void cc::EventQueue::add_timer(uint timerId, TimerConfig config, long data)
{
    change(timerId,EVFILT_TIMER,EV_ADD,config,data);
}

void cc::EventQueue::add_signal(uint signal, intptr_t data)
{
    change(signal,EVFILT_SIGNAL,EV_ADD,0,data);
}

void cc::EventQueue::add_fd_writable(int fd)
{
    change(fd,EVFILT_WRITE,EV_ADD | EV_EOF,0,0);
}

void cc::EventQueue::add_fd_readable(int fd,ReadableConfig config,intptr_t data)
{
    change(fd,EVFILT_READ,EV_ADD | EV_ERROR,config,0);
}

int cc::EventQueue::wait(const struct timespec *timeout,struct kevent& event){
    return kevent(m_kq,nullptr,0,&event,1,timeout);
}

int cc::EventQueue::wait(const struct timespec *timeout,std::vector<struct kevent>& e)
{
    memset(m_event.data(),0,m_event.size() * sizeof(struct kevent));
    e.clear();
    int c = kevent(m_kq,nullptr,0,m_event.data(),m_event.size(),timeout);
    for (size_t i = 0; i < c; i++)
    {
        e.push_back(m_event[i]);
    }
    return c;
}

int  cc::EventQueue::wait(TimeInterval timeout,std::vector<struct kevent>& e){
    int sec = floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nano;
    return wait(&ts,e);
}

int cc::EventQueue::wait(TimeInterval timeout,struct kevent& event){
    int sec = floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nano;
    return wait(&ts,event);   
}

int cc::EventQueue::wait(int sec,std::vector<struct kevent>& event){
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = 0;
    return wait(&ts,event); 
}

int cc::EventQueue::wait(int sec,struct kevent& event){
    timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = 0;
    return wait(&ts,event); 
}

void cc::EventQueue::add_process(pid_t pid,ProcessConfig config){
    add(pid,EVFILT_PROC,config,0);
}

void cc::EventQueue::add_vnode(int fd,VnodeConfig config){
    addClear(fd,EVFILT_VNODE,config,0);
}
