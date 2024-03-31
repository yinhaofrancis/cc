#ifndef STREAM_HPP
#define STREAM_HPP

#include <unistd.h>
#include <fcntl.h>
#include "ref.hpp"
#include "block.hpp"


namespace cc
{
    class Stream{
    public:
        Stream(int fd = 0);
        int Open(const char* path,int flag);
        int Open(const char *path, int flag,mode_t mode);
        ssize_t Write(Block &) const;
        ssize_t Read(Block &) const;
        int Status() const;
        void setStatus(int) const;
        void addStatus(int) const;
        void removeStatus(int) const;
        int Flag() const;
        void setFlag(int) const;
        void addFlag(int) const;
        void removeFlag(int) const;
        int Close() const;
        static int Pipe(Stream& read,Stream& write);
        int fd();
    private:
        int m_fd;
    };
} // namespace cc



#endif