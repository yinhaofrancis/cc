#include "stream.hpp"

cc::Stream::Stream(int fd):m_fd(fd){}

void cc::Stream::Close()
{
    close(m_fd);
}
int cc::Stream::Read(char *buffer, size_t size)
{
    return read(m_fd,buffer,size);
}
int cc::Stream::Write(char *buffer, size_t size)
{
    return write(m_fd,buffer,size);
}
int cc::Stream::Send(char *buffer, size_t size,int flag)
{
    return send(m_fd,buffer,size,flag);
}
int cc::Stream::Recv(char *buffer, size_t size,int flag)
{
    return recv(m_fd,buffer,size,flag);
}