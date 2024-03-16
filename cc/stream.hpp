#ifndef STREAM_HPP
#define STREAM_HPP
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
namespace cc
{
    class Stream
    {
    public:
        Stream(int fd);
        void Close();
        int Read(char* buffer,size_t size);
        int Write(char* buffer,size_t size);
        int Send(char* buffer,size_t size,int flag);
        int Recv(char* buffer,size_t size,int flag);
    private:
        int m_fd;
    };
} // namespace cc


#endif