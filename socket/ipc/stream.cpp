#include "stream.hpp"

ipc::stream::stream(int fd) : fd(fd) {}

size_t ipc::stream::read(void *buf, size_t len)
{
    char *cur = reinterpret_cast<char *>(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::read(fd, cur, space);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            cur += ret;
            space -= ret;
        }
    }
    return len - space;
}

size_t ipc::stream::write(const void *buf, size_t len)
{
    char *cur = (char *)(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::read(fd, cur, space);
        if (ret <= 0)
        {
            if (errno == EINTR && ret < 0)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }else
        {
            cur += ret;
            space -= ret;
        }
    }
    return len - space;
}
int ipc::stream::close(){
    return ::close(fd);
}