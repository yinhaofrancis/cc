#include "epoll.hpp"
#include <fcntl.h>
#include <unistd.h>
int cc::Epoll::wait(double timeinterval)
{
    return epoll_wait(m_epollfd,m_events,MAX_EVENTS,int(timeinterval * 1000));
}

cc::Epoll::Epoll()
{
}

cc::Epoll::~Epoll()
{
    close(m_epollfd);
}

int cc::Epoll::add(int fd,  EVENTS event)
{
    return change(EPOLL_CTL_ADD,fd,event);
}

int cc::Epoll::del(int fd, EVENTS event)
{
    return change(EPOLL_CTL_DEL,fd,event);
}

int cc::Epoll::mod(int fd, EVENTS event)
{
    return change(EPOLL_CTL_MOD,fd,event);
}

int cc::Epoll::change(int op,int fd,epoll_event& event)
{
    return epoll_ctl(m_epollfd,op,fd,&event);
}

int cc::Epoll::change(int op, int fd, EVENTS event)
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = event;
    return change(op,fd,ee);
}
