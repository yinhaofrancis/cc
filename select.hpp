#ifndef SELECT_HPP
#define  SELECT_HPP

#include <sys/select.h>
#include <sys/poll.h>
#include <vector>


#include "time.hpp"

namespace cc
{

    class Select
    {
    public:
        enum Config
        {
            SelectRead = 1,
            SelectWrite = 1 << 1,
            SelectAll = SelectRead | SelectWrite
        };

    public:
        Select();
        ~Select();
        void add(int fd, cc::Select::Config config);
        int wait(timeval &time);
        int wait(TimeInterval seconds);
        void occur_write(std::vector<int> &fd);
        void occur_read(std::vector<int> &fd);
        void occur_error(std::vector<int> &fd);

    private:
        int maxfd = 0;
        std::vector<int> m_fds;
        fd_set m_readfd, m_writefd, m_errorfd;
    };

} // namespace cc

#endif