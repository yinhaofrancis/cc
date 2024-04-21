#include "stream.hpp"

ipc::stream::stream(int fd) : fd(fd) {}

ipc::stream::stream(stream &s) : fd(s.fd)
{
}

ipc::stream::stream(stream &&s) : fd(s.fd)
{
}

size_t ipc::stream::read(void *buf, size_t len) const
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

size_t ipc::stream::write(const void *buf, size_t len) const
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
size_t ipc::stream::send(const void *buf, size_t len) const
{
    char *cur = (char *)(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::send(fd, cur, space, 0);
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
size_t ipc::stream::recv(void *buf, size_t len) const
{
    char *cur = reinterpret_cast<char *>(buf);
    size_t space = len;
    size_t nRead = 0;
    while (space > 0)
    {
        int ret = ::recv(fd, cur, space, 0);
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
ipc::status ipc::stream::status() const
{
    return ipc::status(fcntl(fd, F_GETFL));
}

int ipc::stream::setStatus(ipc::status status) const
{
    return fcntl(fd, F_SETFL, status);
}

int ipc::stream::close() const
{
    return ::close(fd);
}

ipc::stream ipc::stream::dup(int fd)
{
    return stream(::dup(fd));
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

int ipc::file::rlock(lock &lock)
{
    flock l;
    l.l_type = (short)ipc::file::lockopt::read_lock;
    l.l_start = lock.m_start;
    l.l_len = lock.m_len;
    l.l_whence = lock.m_whence;
    return fcntl(fd, F_SETLK, &l);
}

int ipc::file::wlock(lock &lk)
{
    flock l;
    l.l_type = (short)ipc::file::lockopt::write_lock;
    l.l_start = lk.m_start;
    l.l_len = lk.m_len;
    l.l_whence = lk.m_whence;
    return fcntl(fd, F_SETLK, &l);
}

int ipc::file::unlock(lock &lk)
{
    flock l;
    l.l_type = (short)ipc::file::lockopt::ulock;
    l.l_start = lk.m_start;
    l.l_len = lk.m_len;
    l.l_whence = lk.m_whence;
    return fcntl(fd, F_SETLK, &l);
}

pid_t ipc::file::hasLock(lock &lk)
{
    flock l;
    l.l_type = (short)lk.m_opt;
    l.l_start = lk.m_start;
    l.l_len = lk.m_len;
    l.l_whence = lk.m_whence;
    int ret = fcntl(fd,F_GETLK,&l);
    if (ret < 0){
        return ret;
    }
    if(l.l_type == F_UNLCK){
        return 0;
    }
    return l.l_pid;
}

int ipc::file::umask(mode mode)
{
    return ::umask((mode_t)mode);
}

int ipc::file::chmod(mode m)
{
    return fchmod(fd,m);
}

ipc::stream ipc::file::mkfifo(const char *path, mode_t mode)
{
    int fd = ::mkfifo(path, mode);
    return ipc::stream(fd);
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

void ipc::pipe::close() const
{
    m_rpipe->close();
    m_wpipe->close();
    delete m_rpipe;
    delete m_wpipe;
}

const ipc::stream &ipc::pipe::rpipe() const
{
    return *m_rpipe;
}

const ipc::stream &ipc::pipe::wpipe() const
{
    return *m_wpipe;
}

ipc::file::mode ipc::operator|(ipc::file::mode r, ipc::file::mode l)
{
    return (ipc::file::mode)((uint64_t)r | (uint64_t)l);
}
