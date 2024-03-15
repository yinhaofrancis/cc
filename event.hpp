#ifndef EVENT_HPP
#define EVENT_HPP

#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

#include "time.hpp"

namespace cc
{
    const int max_event_count = 10;

    class EventQueue
    {
    public:
        void change(struct kevent &e);

        void change(uintptr_t ident, int16_t filter, uint16_t flag, uint32_t fflag, intptr_t data);

        void add(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data);

        void addOnce(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data);

        void addClear(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data);

        void enable(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data);

        void remove(uintptr_t ident, int16_t filter);

        void disable(uintptr_t ident, int16_t filter);

        int wait(const struct timespec *timeout, std::vector<struct kevent> &);

        int wait(const struct timespec *timeout, struct kevent &event);

        int wait(TimeInterval timeout, std::vector<struct kevent> &);

        int wait(TimeInterval timeout, struct kevent &event);

        int wait(int sec, std::vector<struct kevent> &);

        int wait(int sec, struct kevent &event);

        void add_timer(uint timerId, uint32_t fflag, intptr_t data);

        void add_signal(uint signal, intptr_t data);

        void add_fd_readable(int fd, uint32_t fflag, intptr_t data);

        void add_fd_writable(int fd);
       
        void add_process(pid_t pid, uint32_t fflag);

        void add_vnode(int fd, uint32_t fflag);

    private:
        int m_kq = kqueue();
        std::array<struct kevent, max_event_count> m_event;
    };
} // namespace cc
#endif