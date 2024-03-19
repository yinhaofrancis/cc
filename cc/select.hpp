#ifndef SELECT_HPP
#define SELECT_HPP

#include <sys/select.h>
#include <sys/poll.h>
#include <vector>
#include <cmath>
#include "time.hpp"

namespace cc
{

    class Select
    {
    public:
        enum Config
        {
            SelectRead = 0b01,
            SelectWrite = 0b10,
            SelectAll = SelectRead | SelectWrite
        };
    public:
        Select();
        ~Select();
        void add(int fd, cc::Select::Config config);
        void remove(int fd);
        int wait(timeval &time) const;
        int wait(TimeInterval seconds) const;
        void occur_write(std::vector<int> &fd) const;
        void occur_read(std::vector<int> &fd) const;
        void occur_error(std::vector<int> &fd) const;

    private:
        struct select_item{
            int fd;
            Config config;
        };
    private:

        std::vector<select_item> m_fds;
        fd_set *m_readfd, *m_writefd, *m_errorfd;
    };

    class Poll
    {
    public:
        enum Config{
            ConfigIN          = 0x0001,           /* any readable data available */
            ConfigPRI         = 0x0002,           /* OOB/Urgent readable data */
            ConfigOUT         = 0x0004,           /* file descriptor is writeable */
            ConfigRDNORM      = 0x0040,           /* non-OOB/URG data available */
            ConfigWRNORM      = ConfigOUT,        /* no write type differentiation */
            ConfigRDBAND      = 0x0080,           /* OOB/Urgent readable data */
            ConfigWRBAND      = 0x0100,           /* OOB/Urgent data can be written */
            ConfigERR         = 0x0008,           /* some poll error occurred */
            ConfigHUP         = 0x0010,           /* file descriptor was "hung up" */
            ConfigNVAL        = 0x0020,           /* requested events "invalid" */
        };
        struct PollResult
        {
            int fd;
            Config config;
        };
    public:
        Poll();
        ~Poll();
        Poll(Poll&) = delete;
        Poll(Poll&&) = delete;
        void    add(int fd,Config config);
        void    remove(int fd);
        int     wait(TimeInterval seconds) const;
        void    occur(std::vector<PollResult> &result) const;
    private:    
        std::vector<pollfd> *m_poll_fd;
        
    };

} // namespace cc

#endif