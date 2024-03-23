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

#include "object.hpp" 

namespace cc
{
    enum AddressFamily{
        Ipv4 = AF_INET ,Ipv6 = AF_INET6
    };

    class Block:public Object{
    public:
        Block(const void* buffer,const size_t size);
        ~Block();
        virtual void    dealloc();
        const void*     buffer();
        const size_t    size();        
    private:
       const void* m_buffer;
       const size_t m_size;
    };

    class EndPoint{
    public:
        static int resolute(std::string host,int socktype,std::vector<EndPoint>& endpoint);

        EndPoint(const struct sockaddr *,socklen_t);
        EndPoint(const EndPoint&);
        EndPoint(const EndPoint&&);
        void operator = (EndPoint&);
        void operator = (EndPoint&&);
        EndPoint(AddressFamily af,std::string ip,uint16_t port);
        EndPoint(AddressFamily af,uint16_t port);
        ~EndPoint();
        struct sockaddr* address() const;
        uint16_t port() const;
        void set_port(uint16_t port);
        socklen_t address_len() const;
        socklen_t& address_len();
        void info(std::string &ipInfo) const;
        void ipAddressAndPort(std::string& ip,uint16_t& port) const;
        
        friend class Stream;
    private:
        struct sockaddr *m_address;
        socklen_t m_address_len;
    };

    class Stream
    {
    public:
        Stream(int fd);
        Stream();
        void Close() const;
        ssize_t Read(void* buffer,size_t size) const;
        ssize_t Write(void* buffer,size_t size) const;
        ssize_t Send(const void* buffer,const size_t size,int flag) const;
        ssize_t Recv(void* buffer,size_t size,int flag) const;
        ssize_t SendTo(const void* buffer, const size_t size,int flag,const EndPoint&) const;
        ssize_t RecvFrom(void* buffer,size_t size,int flag,EndPoint&) const;
        int& streamFD();
        int Listen(int backlog) const;
        int Accept(EndPoint& ep,cc::Stream& stream) const;
        int Bind(const EndPoint&) const;
        int Connect(const EndPoint&) const;
        void setNoBlock() const;
        static int CreateTcp(AddressFamily af,Stream&);
        static int CreateUdp(AddressFamily af,Stream&);
    private:
        int m_fd;
    };
} // namespace cc


#endif