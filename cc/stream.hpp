#ifndef STREAM_HPP
#define STREAM_HPP
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
namespace cc
{
    class Stream
    {
    public:
        Stream(int fd);
        void Close();
        size_t Read(char* buffer,size_t size);
        size_t Write(char* buffer,size_t size);
        size_t Send(char* buffer,size_t size,int flag);
        size_t Recv(char* buffer,size_t size,int flag);
        size_t SendTo(char* buffer,size_t size,int flag,const struct sockaddr *, socklen_t);
        size_t RecvFrom(char* buffer,size_t size,int flag, struct sockaddr *, socklen_t*);
        int& streamFD();
        int Listen(int backlog);
        Stream Accept(struct sockaddr *, socklen_t*);
        int Bind(const struct sockaddr *, socklen_t);
        int Connect(const struct sockaddr *, socklen_t);
    private:
        int m_fd;
    };

    class EndPoint{
    public:
        static int resolute(std::string host,int16_t port,int socktype,std::vector<EndPoint>& endpoint);
        EndPoint(const struct sockaddr *, socklen_t,int16_t port);
        const struct sockaddr& address();
        int16_t& port();
    private:
        struct sockaddr m_address;
        socklen_t m_addrlen = 0;
        int16_t m_port;
    };


} // namespace cc


#endif