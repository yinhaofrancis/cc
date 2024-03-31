#include "epoll.hpp"
#include <fcntl.h>
#include <unistd.h>


int cc::Epoll::wait(double timeinterval, std::vector<Result> &res) const
{
    struct epoll_event buffer[MAX_EVENTS];
    
    int ret = epoll_wait(m_epollfd,buffer,MAX_EVENTS,int(timeinterval * 1000));
    res.clear();
    for (size_t i = 0; i < ret; i++)
    {
        Result r;
        r.ident = buffer[i].data.fd;
        r.events = static_cast<Event>(buffer[i].events);
        res.push_back(r);
    }
    return ret;
}

cc::Epoll::Epoll()
{
}

cc::Epoll::~Epoll()
{
    close(m_epollfd);
}

int cc::Epoll::add(int fd,  Event event) const
{
    return change(EPOLL_CTL_ADD,fd,event);
}

int cc::Epoll::del(int fd, Event event) const 
{
    return change(EPOLL_CTL_DEL,fd,event);
}

int cc::Epoll::mod(int fd, Event event) const 
{
    return change(EPOLL_CTL_MOD,fd,event);
}

int cc::Epoll::change(int op,int fd,epoll_event& event) const
{
    return epoll_ctl(m_epollfd,op,fd,&event);
}

int cc::Epoll::change(int op, int fd, Event event) const
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = event;
    return change(op,fd,ee);
}
