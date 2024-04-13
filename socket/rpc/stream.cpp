#include "stream.hpp"

rpc::stream::stream(int fd) : fd(fd) {}

rpc::stream::stream(stream & s):fd(s.fd)
{}

rpc::stream::stream(stream && s):fd(s.fd)
{}

size_t rpc::stream::read(void *buf, size_t len) const
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

size_t rpc::stream::write(const void *buf, size_t len) const
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
size_t rpc::stream::send(const void *buf, size_t len) const
{
    char *cur = (char *)(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::send(fd, cur, space,0);
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
size_t rpc::stream::recv(void *buf, size_t len) const
{
    char *cur = reinterpret_cast<char *>(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::recv(fd, cur, space,0);
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
rpc::status rpc::stream::status() const
{
    return rpc::status(fcntl(fd, F_GETFD));
}

int rpc::stream::setStatus(rpc::status status) const
{
    return fcntl(fd, F_GETFD, status);
}

int rpc::stream::close() const
{
    return ::close(fd);
}

rpc::file::file(const char *path, rpc::status status) : stream(open(path, status))
{
}

off_t rpc::file::seek(off_t offset, whence w)
{
    return ::lseek(fd, offset, w);
}

int rpc::file::truncate(off_t offset)
{
    return ::ftruncate(fd, offset);
}

int rpc::file::umask(mode_t mode)
{
    return umask(mode);
}

int rpc::file::mkfifo(const char *path, mode_t mode)
{
    return ::mkfifo(path,mode);
}

rpc::status operator|(rpc::status v1, rpc::status v2)
{
    return rpc::status((int)v1 | (int)v2);
}

rpc::pipe::pipe()
{
    int fds[2];
    ::pipe(fds);
    m_rpipe = new rpc::stream(fds[0]);
    m_wpipe = new rpc::stream(fds[1]);
}

void rpc::pipe::close() const
{
    m_rpipe->close();
    m_wpipe->close();
    delete m_rpipe;
    delete m_wpipe;
}

const rpc::stream &rpc::pipe::rpipe() const
{
    return *m_rpipe;
}

const rpc::stream &rpc::pipe::wpipe() const
{
    return *m_wpipe;
}
