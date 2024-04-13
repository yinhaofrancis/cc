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
        int ret = ::write(fd, cur, space);
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
        }
        else
        {
            cur += ret;
            space -= ret;
        }
    }
    return len - space;
}
ipc::status ipc::stream::status()
{
    return ipc::status(fcntl(fd, F_GETFD));
}

int ipc::stream::setStatus(ipc::status status)
{
    return fcntl(fd, F_GETFD, status);
}

int ipc::stream::close()
{
    return ::close(fd);
}

ipc::file::file(const char *path, ipc::status status) : stream(open(path, status))
{
}

off_t ipc::file::seek(off_t offset, whence w)
{
    return ::lseek(fd, offset, w);
}

int ipc::file::truncate(off_t offset)
{
    return ::ftruncate(fd, offset);
}

void ipc::file::umask(mode_t mode)
{
    umask(mode);
}

ipc::status operator|(ipc::status v1, ipc::status v2)
{
    return ipc::status((int)v1 | (int)v2);
}

