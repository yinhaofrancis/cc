#include "select.hpp"
#include <algorithm>
cc::Select::Select() {
    FD_ZERO(&m_readfd);
    FD_ZERO(&m_writefd);
    FD_ZERO(&m_errorfd);
}

cc::Select::~Select() {}

void cc::Select::add(int fd,cc::Select::Config config)
{
    maxfd = std::max(maxfd, fd);
    if(config & SelectRead){
        FD_SET(fd, &m_readfd);
    }
    if(config & SelectWrite){
        FD_SET(fd, &m_writefd);
    }
     FD_SET(fd, &m_errorfd);
    m_fds.push_back(fd);
}

int cc::Select::wait(timeval &timeout)
{
    return select(maxfd + 1, &m_readfd, &m_writefd, &m_errorfd, &timeout);
}
int cc::Select::wait(TimeInterval timeout)
{
    int sec = floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000 * 1000;
    timeval m {sec,nano};
    return cc::Select::wait(m);
}

void cc::Select::occur_write(std::vector<int> &fd)
{
    for (auto i =  m_fds.begin() ; i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i,&m_writefd))
        {
            fd.push_back(*i);
        }
    }
}
void cc::Select::occur_read(std::vector<int> &fd)
{
    for (auto i =  m_fds.begin() ; i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i,&m_readfd))
        {
            fd.push_back(*i);
        }
    }
}
void cc::Select::occur_error(std::vector<int> &fd)
{
    for (auto i =  m_fds.begin() ; i < m_fds.end(); i++)
    {
        if (FD_ISSET(*i,&m_errorfd))
        {
            fd.push_back(*i);
        }
    }
}