#include "stream.hpp"
#include "errno.h"
#include <string.h>
#include <iostream>
cc::Stream::Stream(int fd):m_fd(fd){
}

int cc::Stream::Open(const char *path, int flag,mode_t mode)
{
    
    int ret = open(path,flag,mode);
    if(ret < 0){
        std::cout << strerror(errno)<<std::endl;
    }
    m_fd = ret;
    return ret;
}
int cc::Stream::Open(const char *path, int flag)
{
    return Open(path,flag,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

ssize_t cc::Stream::Write(Block & block) const
{
    return write(m_fd,block.data(),block.size());
}

ssize_t cc::Stream::Read(Block &block) const
{

    ssize_t size = read(m_fd,const_cast<void *>(block.data()),block.size());
    if(size > 0){
        block.m_size = size;
    }
    return size;
}

int cc::Stream::Status() const
{
    return fcntl(m_fd,F_GETFL);
}

void cc::Stream::setStatus(int flag) const
{
    fcntl(m_fd,F_SETFL,flag);
}

void cc::Stream::addStatus(int flag) const
{
    int cflag = Status();
    cflag = cflag | flag;
    setStatus(cflag);
}

void cc::Stream::removeStatus(int flag) const{
    int cflag = Status();
    cflag = cflag & (~flag);
    setStatus(cflag);
}

int cc::Stream::Flag() const
{
    return fcntl(m_fd,F_GETFD);
}

void cc::Stream::setFlag(int flag) const
{
    fcntl(m_fd,F_SETFD,flag);
}

void cc::Stream::addFlag(int flag) const
{
    int cflag = Flag();
    cflag = cflag | flag;
    setFlag(cflag);
}

void cc::Stream::removeFlag(int flag) const
{
    int cflag = Status();
    cflag = cflag & (~flag);
    setFlag(cflag);
}

int cc::Stream::Close() const
{
    if(m_fd > 0){
        return close(m_fd);
    }
    return 0;
}

int cc::Stream::Pipe(Stream &read, Stream &write)
{
    int fd[2];
    int ret = pipe(fd);
    read.m_fd = fd[0];
    write.m_fd = fd[1];
    return ret;
}

int cc::Stream::fd()
{
    return m_fd;
}
