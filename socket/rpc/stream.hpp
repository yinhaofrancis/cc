#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "define.hpp"

namespace rpc
{

    class stream
    {
    public:
        const int fd;
        stream(int fd);
        stream(stream&);
        stream(stream&&);
        size_t read(void *buf, size_t len) const ;
        size_t write(const void *buf, size_t len) const;
        size_t send(const void *buf, size_t len) const;
        size_t recv(void *buf, size_t len) const;
        rpc::status status() const;
        int setStatus(rpc::status status) const;
        int close() const;
    };

    class pipe{
    public:
        pipe();
        void close() const;
        const rpc::stream& rpipe() const;
        const rpc::stream& wpipe() const;
    private:
        rpc::stream *m_rpipe = nullptr;
        rpc::stream *m_wpipe = nullptr;
    };


    class file:public stream{
    public:
        enum whence{
            seekset = SEEK_SET,
            seekcur = SEEK_CUR,
            seekend = SEEK_END
        };
    public:
        file(const char *path,rpc::status status);
        off_t seek(off_t offset,whence w);
        int truncate(off_t offset);
        static int umask(mode_t mode);
        static int mkfifo(const char* path,mode_t mode);
    };
} // namespace rpc

#endif