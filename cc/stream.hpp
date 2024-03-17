#ifndef STREAM_HPP
#define STREAM_HPP
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <cstring>
namespace cc
{

    class EndPoint{
    public:
        static int resolute(std::string host,int socktype,std::vector<EndPoint>& endpoint);
        EndPoint(const struct sockaddr *,socklen_t);
        EndPoint(const EndPoint&);
        EndPoint(const EndPoint&&);
        EndPoint(int af,std::string ip,uint16_t port);
        ~EndPoint();
        struct sockaddr* address() const;
        uint16_t port() const;
        void set_port(uint16_t port);
        socklen_t address_len() const;
        socklen_t& address_len();
        std::string info();
        friend class Stream;
    private:
        struct sockaddr *m_address;
        socklen_t m_address_len;
    };

    class Stream
    {
    public:
        Stream(int fd);
        void Close();
        size_t Read(char* buffer,size_t size);
        size_t Write(char* buffer,size_t size);
        size_t Send(char* buffer,size_t size,int flag);
        size_t Recv(char* buffer,size_t size,int flag);
        size_t SendTo(char* buffer,size_t size,int flag,const EndPoint&);
        size_t RecvFrom(char* buffer,size_t size,int flag,EndPoint&);
        int& streamFD();
        int Listen(int backlog);
        Stream Accept(EndPoint&);
        int Bind(const EndPoint&);
        int Connect(const EndPoint&);
        
    private:
        int m_fd;
    };
} // namespace cc


#endif