#include "poll.hpp"

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
    m_pfd->push_back((pollfd){fd, (short)event, 0});
}

void cc::Poll::remove(int fd, Event event)
{
    m_pfd->erase(std::find_if(m_pfd->begin(), m_pfd->end(), [fd, event](pollfd &pfd)
                             { return pfd.fd == fd && pfd.events == event; }));
}
void cc::Poll::remove(int fd)
{
    m_pfd->erase(std::find_if(m_pfd->begin(), m_pfd->end(), [fd](pollfd &pfd)
                             { return pfd.fd == fd; }));
}
cc::Poll::Poll()
{
    m_pfd = new std::vector<pollfd>();
}