#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace ipc
{

    class stream
    {
    public:
        const int fd;
        stream(int fd);
        size_t read(void *buf, size_t len);
        size_t  write(const void *buf, size_t len);
        int close();
    };
} // namespace ipc


#endif