#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "define.hpp"

namespace ipc
{

    class stream
    {
    public:
        const int fd;
        stream(int fd);
        size_t read(void *buf, size_t len);
        size_t write(const void *buf, size_t len);
        ipc::status status();
        int setStatus(ipc::status status);
        int close();
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
    };
} // namespace ipc

#endif