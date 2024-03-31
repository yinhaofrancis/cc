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
            IN		= 0x001,		/* There is data to read.  */
            PRI		= 0x002,		/* There is urgent data to read.  */
            OUT		= 0x004,		/* Writing now will not block.  */
            RDNORM	= 0x040,		/* Normal data may be read.  */
            RDBAND	= 0x080,		/* Priority data may be read.  */
            WRNORM	= 0x100,		/* Writing now will not block.  */
            WRBAND	= 0x200,		/* Priority data may be written.  */
            MSG	    = 0x400,
            REMOVE  = 0x1000,
            RDHUP	= 0x2000,
            ERR		= 0x008,		/* Error condition.  */
            HUP		= 0x010,		/* Hung up.  */
            NVAL	= 0x020,	
        };
        struct Result{
            int fd; Event events,revents;
        };
    public:
        Poll();
        ~Poll();
        int wait(TimeInterval time,std::vector<Result> &events) const;
        void add(int fd,Event event);
        void remove(int fd,Event event);
        void remove(int fd);
    private:
        std::vector<pollfd> *m_pfd;
    };
} // namespace cc

#endif