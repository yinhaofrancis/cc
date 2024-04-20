#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "define.hpp"

namespace ipc
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
        ipc::status status() const;
        int setStatus(ipc::status status) const;
        int close() const;
        static stream dup(int fd);
    };

    class pipe{
    public:
        pipe();
        void close() const;
        const ipc::stream& rpipe() const;
        const ipc::stream& wpipe() const;
    private:
        ipc::stream *m_rpipe = nullptr;
        ipc::stream *m_wpipe = nullptr;
    };


    class file:public stream{
    public:
        enum whence{
            seekset = SEEK_SET,
            seekcur = SEEK_CUR,
            seekend = SEEK_END
        };
    public:
        file(const char *path,ipc::status status);
        off_t seek(off_t offset,whence w);
        int truncate(off_t offset);
        static int umask(mode_t mode);
        static int mkfifo(const char* path,mode_t mode);
    };
} // namespace ipc

#endif