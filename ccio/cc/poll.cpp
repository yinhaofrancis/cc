#include "poll.hpp"

cc::Poll::~Poll()
{
}

int cc::Poll::wait(TimeInterval time)
{
    return poll(m_pfd.data(), m_pfd.size(), time * 1000);
}

void cc::Poll::add(int fd, Event event)
{
    m_pfd.push_back((pollfd){fd, (short)event, 0});
}

void cc::Poll::remove(int fd, Event event)
{
    m_pfd.erase(std::find_if(m_pfd.begin(), m_pfd.end(), [fd, event](pollfd &pfd)
                             { return pfd.fd == fd && pfd.events == event; }));
}
void cc::Poll::remove(int fd)
{
    m_pfd.erase(std::find_if(m_pfd.begin(), m_pfd.end(), [fd](pollfd &pfd)
                             { return pfd.fd == fd; }));
}
cc::Poll::Poll()
{
}