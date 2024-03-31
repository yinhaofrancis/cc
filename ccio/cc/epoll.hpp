#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <vector>

namespace cc
{

    const int MAX_EVENTS = 10;
    class Epoll
    {
    public:
        enum Event
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
        struct Result
        {
            uint64_t ident;
            Event events;
        };
        

    public:
        int wait(double timeinterval,std::vector<Result>& res) const;
        Epoll(const Epoll &) = delete;
        Epoll(const Epoll &&) = delete;
        Epoll();
        ~Epoll();
        int add(int fd, Event event) const;
        int del(int fd, Event event) const;
        int mod(int fd, Event event) const;

    private:
        int m_epollfd = epoll_create1(0);
        int change(int op, int fd, epoll_event &event) const;
        int change(int op, int fd, Event event) const;
    };
} // namespace cc

#endif