#ifndef EVENT_HPP
#define EVENT_HPP

#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
namespace cc
{
    typedef double TimeInterval;

    const int max_event_count = 10;

    class EventQueue
    {
    public:
        enum ReadableConfig
        {
            LOWAT = NOTE_LOWAT
        };
        enum TimerConfig
        {
            SECONDS = NOTE_SECONDS,
            USECONDS = NOTE_USECONDS,
            NSECONDS = NOTE_NSECONDS,
            MACHTIME = NOTE_MACHTIME,
            LEEWAY = NOTE_LEEWAY,        /* ext[1] holds leeway for power aware timers */
            CRITICAL = NOTE_CRITICAL,    /* system does minimal timer coalescing */
            BACKGROUND = NOTE_BACKGROUND /* system does maximum timer coalescing */

        };
        enum ProcessConfig
        {
            EXIT = NOTE_EXIT,
            EXITSTATUS = NOTE_EXITSTATUS,
            FORK = NOTE_FORK,
            EXEC = NOTE_EXEC,
            SIGNAL = NOTE_SIGNAL
        };
        enum VnodeConfig
        {
            DELETE = NOTE_DELETE,
            WRITE = NOTE_WRITE,
            EXTEND = NOTE_EXTEND,
            ATTRIB = NOTE_ATTRIB,
            LINK = NOTE_LINK,
            RENAME = NOTE_RENAME,
            REVOKE = NOTE_REVOKE,
            FUNLOCK = NOTE_FUNLOCK,
            LEASE_DOWNGRADE = NOTE_LEASE_DOWNGRADE,
            LEASE_RELEASE = NOTE_LEASE_RELEASE
        };

    public:

        int change(uintptr_t ident, int16_t filter, uint16_t flag, uint32_t fflag, intptr_t data) const;

        int add(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const;

        int addOnce(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const;

        int addClear(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const;

        int enable(uintptr_t ident, int16_t filter, uint32_t fflag, intptr_t data) const;

        int remove(uintptr_t ident, int16_t filter) const;

        int disable(uintptr_t ident, int16_t filter) const;

        int wait(const struct timespec *timeout, std::vector<struct kevent> &) const;

        int wait(const struct timespec *timeout, struct kevent &event) const;

        int wait(TimeInterval timeout, std::vector<struct kevent> &) const;

        int wait(TimeInterval timeout, struct kevent &event) const;

        int wait(int sec, std::vector<struct kevent> &) const;

        int wait(int sec, struct kevent &event) const;

        int add_timer(uint timerId, TimerConfig config, long data) const;

        int add_signal(uint signal, intptr_t data) const;

        int add_fd_readable(int fd, ReadableConfig fflag) const;

        int add_fd_writable(int fd) const;

        int add_process(pid_t pid, ProcessConfig fflag) const;

        int add_vnode(int fd, VnodeConfig config) const;
    private:
        int change(struct kevent& change) const;
    private:
        int m_kq = kqueue();
        std::unique_ptr<std::array<struct kevent, max_event_count>> m_event;

    };

} // namespace cc

#endif