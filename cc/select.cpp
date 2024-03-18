#include "select.hpp"
#include <algorithm>
#include <vector>
cc::Select::Select()
{
    FD_ZERO(&m_readfd);
    FD_ZERO(&m_writefd);
    FD_ZERO(&m_errorfd);
}

cc::Select::~Select() {}

void cc::Select::add(int fd, cc::Select::Config config)
{
    if(config & cc::Select::Config::SelectWrite){
        FD_SET(fd,&m_writefd);
    }
    if(config & cc::Select::Config::SelectRead){
        FD_SET(fd,&m_readfd);
    }
    FD_SET(fd,&m_errorfd);
    m_fds.push_back(fd);
}

void cc::Select::remove(int fd)
{
    FD_CLR(fd,&m_readfd);
    FD_CLR(fd,&m_errorfd);
    FD_CLR(fd,&m_writefd);
    m_fds.erase(std::remove(m_fds.begin(), m_fds.end(),fd), m_fds.end());
}

int cc::Select::wait(timeval &timeout)
{
    return select(maxfd, &m_readfd, &m_writefd, &m_errorfd, &timeout);
}
int cc::Select::wait(TimeInterval timeout)
{
    int sec = std::floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timeval m{sec, nano};
    return cc::Select::wait(m);
}

void cc::Select::occur_write(std::vector<int> &fd)
{
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i, &m_writefd))
        {
            fd.push_back(*i);
        }
    }
}
void cc::Select::occur_read(std::vector<int> &fd)
{
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i, &m_readfd))
        {
            fd.push_back(*i);
        } 
    }
}
void cc::Select::occur_error(std::vector<int> &fd)
{
    
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i, &m_errorfd))
        {
            fd.push_back(*i);
        }
    }
}

cc::Poll::Poll()
{
}
cc::Poll::~Poll()
{
}
void cc::Poll::add(int fd, Config config)
{
    pollfd p_fd;
    p_fd.fd = fd;
    p_fd.events = config;
    p_fd.revents = 0;
    m_poll_fd.push_back(p_fd);    
}

void cc::Poll::remove(int fd)
{
    m_poll_fd.erase(std::remove_if(m_poll_fd.begin(), m_poll_fd.end(), [fd](const pollfd &p) {
        return p.fd == fd;
    }), m_poll_fd.end());
}

int cc::Poll::wait(TimeInterval seconds)
{
    return poll((pollfd*) m_poll_fd.data(),m_poll_fd.size(),seconds * 1000);
}
void cc::Poll::occur(std::vector<PollResult> &result){
    for (auto i = m_poll_fd.begin(); i < m_poll_fd.end(); i++)
    {
        if(i->revents != 0){
            PollResult pr;
            pr.fd = i->fd;
            pr.config = (Config)i->revents;
            result.push_back(pr);
            i->revents = 0;
        }
    }
}