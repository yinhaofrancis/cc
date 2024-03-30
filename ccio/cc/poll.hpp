#ifndef POLL_HPP
#define POLL_HPP
#include <unordered_map>
#include <vector>
#include <signal.h>
#include <sys/poll.h>
typedef double TimeInterval;
namespace cc
{
    class Poll
    {
    public:
        enum Event
        {

        };

    public:
        Poll();
        ~Poll();
        int wait(TimeInterval time);
        void add(int fd,short event);
        void remove(int fd,int event);
        void remove(int fd);
    private:
        std::vector<pollfd> m_pfd;
    };
} // namespace cc

#endif