#include "poll.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

cc::Poll::~Poll()
{
    delete m_pfd;
}

int cc::Poll::wait(TimeInterval time,std::vector<Result>& pfds) const
{
    
    int r = poll(const_cast<pollfd *>(m_pfd->data()), m_pfd->size(), time * 1000);
    if (r > 0){
        for (auto &&i : *m_pfd)
        {
            if (i.revents != 0){
                Result r;
                r.fd = i.fd;
                r.events = static_cast<cc::Poll::Event>(i.events);
                r.revents = static_cast<cc::Poll::Event>(i.revents);
                pfds.push_back(r);
                short *s = &i.revents;
                *s = 0;
            }
        }
        
    }
    return r;
}

void cc::Poll::add(int fd, Event event)
{
    auto iidex = std::find_if(m_pfd->begin(),m_pfd->end(),[fd](pollfd& i){return i.fd == fd;});

    if(iidex != m_pfd->end()){
        iidex->events |= event;
    }else{
        m_pfd->push_back((pollfd){fd, (short)event, 0});
    }
    
}

void cc::Poll::remove(int fd, Event event)
{
    auto iidex = std::find_if(m_pfd->begin(),m_pfd->end(),[fd](pollfd& i){return i.fd == fd;});

    if(iidex != m_pfd->end()){
        iidex->events &= ~event;
    }
}
void cc::Poll::remove(int fd)
{
    auto it = std::find_if(m_pfd->begin(), m_pfd->end(), [fd](pollfd &pfd)
                             { return pfd.fd == fd; });
    if(it != m_pfd->end()){
        m_pfd->erase(it);
    }
}
cc::Poll::Poll()
{
    m_pfd = new std::vector<pollfd>();
}

cc::AsyncPoll::AsyncPoll()
{
    m_loop = new Loop();
    m_loop->dispatch([this](){
        this->m_is_running = true;
        while (this->m_is_running)
        {
            std::vector<Result> events;
            int r = this->wait(0.1,events);
            if(!this->m_is_running){
                return;
            }
            if(r == -1){
                std::cout << strerror(errno) << std::endl;
            }
            if(r > 0){
                for (auto &event : events)
                {
                    auto it = this->m_handle.find(event.fd);
                    if(it != this->m_handle.end()){
                        auto a = it->second;
                        a->onEvent(*this,event);
                    }
                }
            }
        }
        
    });
}

cc::AsyncPoll::~AsyncPoll()
{
    delete m_loop;
}

void cc::AsyncPoll::add(int fd, Event event, AsyncPollCallback *c)
{
    m_lock.lock();
    Poll::add(fd,event);
    m_handle[fd] = c;
    m_lock.unlock();
}

void cc::AsyncPoll::remove(int fd, Event event)
{
    m_lock.lock();
    Poll::remove(fd,event);
    m_lock.unlock();
}
void cc::AsyncPoll::remove(int fd)
{
    m_lock.lock();
    Poll::remove(fd);
    m_handle.erase(fd);
    m_lock.unlock();
}

void cc::AsyncPoll::AsyncPollCallback::onEvent(AsyncPoll &poll, Result &)
{}
