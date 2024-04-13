#include "stream.hpp"

ipc::stream::stream(int fd) : fd(fd) {}

ipc::stream::stream(stream & s):fd(s.fd)
{}

ipc::stream::stream(stream && s):fd(s.fd)
{}

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

int ipc::file::umask(mode_t mode)
{
    return umask(mode);
}

int ipc::file::mkfifo(const char *path, mode_t mode)
{
    return ::mkfifo(path,mode);
}

ipc::status operator|(ipc::status v1, ipc::status v2)
{
    return ipc::status((int)v1 | (int)v2);
}

ipc::pipe::pipe()
{
    int fds[2];
    ::pipe(fds);
    m_rpipe = new ipc::stream(fds[0]);
    m_wpipe = new ipc::stream(fds[1]);
}

void ipc::pipe::close()
{
    m_rpipe->close();
    m_wpipe->close();
    delete m_rpipe;
    delete m_wpipe;
}

const ipc::stream &ipc::pipe::rpipe()
{
    return *m_rpipe;
}

const ipc::stream &ipc::pipe::wpipe()
{
    return *m_wpipe;
}
