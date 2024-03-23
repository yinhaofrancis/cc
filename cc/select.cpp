#include "select.hpp"
#include <algorithm>
#include <vector>
cc::Select::Select()
{
    m_readfd = new fd_set();
    m_writefd = new fd_set();
    m_errorfd = new fd_set();
    FD_ZERO(m_readfd);
    FD_ZERO(m_writefd);
    FD_ZERO(m_errorfd);
}

cc::Select::~Select()
{
    delete m_readfd;
    delete m_errorfd;
    delete m_writefd;
}

void cc::Select::add(int fd, cc::Select::Config config)
{
    auto it = std::find_if(m_fds.begin(), m_fds.end(), [fd, config](select_item &si)
                           { return si.fd == fd && si.config == config; });
    if (it == m_fds.end())
    {
        select_item m;
        m.fd = fd;
        m.config = config;
        m_fds.push_back(m);
    }
}

void cc::Select::remove(int fd)
{
    FD_CLR(fd, m_readfd);
    FD_CLR(fd, m_errorfd);
    FD_CLR(fd, m_writefd);
    m_fds.erase(std::remove_if(m_fds.begin(), m_fds.end(), [fd](const select_item &sfd)
                               { return sfd.fd == fd; }),
                m_fds.end());
}
void cc::Select::remove(int fd, cc::Select::Config config)
{
    FD_CLR(fd, m_readfd);
    FD_CLR(fd, m_errorfd);
    FD_CLR(fd, m_writefd);
    m_fds.erase(std::remove_if(m_fds.begin(), m_fds.end(), [fd, config](const select_item &sfd)
                               { return sfd.fd == fd && config == sfd.config; }),
                m_fds.end());
}

int cc::Select::wait(timeval &timeout) const
{
    FD_ZERO(m_errorfd);
    FD_ZERO(m_writefd);
    FD_ZERO(m_readfd);
    int maxfd = 0;
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (i->config & cc::Select::Config::SelectRead)
        {
            FD_SET(i->fd, m_readfd);
        }
        if (i->config & cc::Select::Config::SelectWrite)
        {
            FD_SET(i->fd, m_writefd);
        }
        FD_SET(i->fd, m_errorfd);
        maxfd = std::max(maxfd, i->fd);
    }
    return select(maxfd + 1, m_readfd, m_writefd, m_errorfd, &timeout);
}
int cc::Select::wait(TimeInterval timeout) const
{
    int sec = std::floor(timeout);
    int nano = (timeout - sec) * 1000 * 1000;
    timeval m{sec, nano};
    return cc::Select::wait(m);
}

void cc::Select::occur_write(std::vector<int> &fd) const
{
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(i->fd, m_writefd))
        {
            fd.push_back(i->fd);
        }
    }
}
void cc::Select::occur_read(std::vector<int> &fd) const
{
    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(i->fd, m_readfd))
        {
            fd.push_back(i->fd);
        }
    }
}
void cc::Select::occur_error(std::vector<int> &fd) const
{

    for (auto i = m_fds.begin(); i < m_fds.end(); i++)
    {
        if (FD_ISSET(i->fd, m_errorfd))
        {
            fd.push_back(i->fd);
        }
    }
}

cc::Poll::Poll()
{
    m_poll_fd = new std::vector<pollfd>();
}
cc::Poll::~Poll()
{
    delete m_poll_fd;
}
void cc::Poll::add(int fd, Config config)
{

    bool it = false;
    for (auto &&i : *m_poll_fd)
    {
        it = i.fd == fd && i.events == config;
        if (it){
            break;
        }
    }
    
    if(it){
        pollfd p_fd;
        p_fd.fd = fd;
        p_fd.events = config;
        p_fd.revents = 0;
        m_poll_fd->push_back(p_fd);
    }
}

void cc::Poll::remove(int fd)
{
    m_poll_fd->erase(std::remove_if(m_poll_fd->begin(), m_poll_fd->end(), [fd](const pollfd &p)
                                    { return p.fd == fd; }),
                     m_poll_fd->end());
}

void cc::Poll::remove(int fd, Config config)
{
    m_poll_fd->erase(std::remove_if(m_poll_fd->begin(), m_poll_fd->end(), [fd,config](const pollfd &p)
                                    { return p.fd == fd && p.events == config; }),
                     m_poll_fd->end());
}

int cc::Poll::wait(TimeInterval seconds) const
{
    return poll((pollfd *)m_poll_fd->data(), m_poll_fd->size(), seconds * 1000);
}
void cc::Poll::occur(std::vector<PollResult> &result) const
{
    for (auto i = m_poll_fd->begin(); i < m_poll_fd->end(); i++)
    {
        if (i->revents != 0)
        {
            PollResult pr;
            pr.fd = i->fd;
            pr.config = (Config)i->revents;
            result.push_back(pr);
            i->revents = 0;
        }
    }
}