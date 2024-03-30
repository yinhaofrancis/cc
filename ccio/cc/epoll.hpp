#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>

namespace cc
{

    const int MAX_EVENTS = 10;
    class Epoll
    {
    public:
        enum EVENTS
        {
            IN = 0x001,
            PRI = 0x002,
            OUT = 0x004,
            RDNORM = 0x040,
            RDBAND = 0x080,
            WRNORM = 0x100,
            LWRBAND = 0x200,
            MSG = 0x400,
            ERR = 0x008,
            HUP = 0x010,
            RDHUP = 0x2000,
            EXCLUSIVE = 1u << 28,
            WAKEUP = 1u << 29,
            ONESHOT = 1u << 30,
            ET = 1u << 31
        };

    public:
        int wait(double timeinterval);
        Epoll(const Epoll &) = delete;
        Epoll(const Epoll &&) = delete;
        Epoll();
        ~Epoll();
        int add(int fd, EVENTS event);
        int del(int fd, EVENTS event);
        int mod(int fd, EVENTS event);

    private:
        int m_epollfd = epoll_create1(0);
        int change(int op, int fd, epoll_event &event);
        int change(int op, int fd, EVENTS event);
        struct epoll_event m_events[MAX_EVENTS];
    };
} // namespace cc

#endif