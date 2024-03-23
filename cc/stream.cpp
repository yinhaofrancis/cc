#include "stream.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

cc::Stream::Stream(int fd) : m_fd(fd) {}

cc::Stream::Stream() : m_fd(0) {}

void cc::Stream::Close() const
{
    close(m_fd);
}
ssize_t cc::Stream::Read(void *buffer, size_t size) const
{
    return read(m_fd, buffer, size);
}
ssize_t cc::Stream::Write(void *buffer, size_t size) const 
{
    return write(m_fd, buffer, size);
}
ssize_t cc::Stream::Send(const void *buffer, const size_t size, int flag) const
{
    return send(m_fd, buffer, size, flag);
}
ssize_t cc::Stream::Recv(void *buffer, size_t size, int flag) const
{
    return recv(m_fd, buffer, size, flag);
}

ssize_t cc::Stream::SendTo(const void *buffer, const size_t size, int flag,const EndPoint& ep) const
{
    return sendto(m_fd, buffer, size, flag, ep.address(), ep.address_len());
}
ssize_t cc::Stream::RecvFrom(void *buffer, size_t size, int flag,EndPoint& ep) const
{
    return  recvfrom(m_fd, buffer, size, flag, ep.address(), &ep.address_len());
}
int cc::Stream::Listen(int backlog) const 
{
    return listen(m_fd, backlog);
}
int cc::Stream::Accept(EndPoint& ep,cc::Stream& stream) const 
{
    auto fd = accept(m_fd, ep.address(), &ep.address_len());
    if (fd > 0){
        stream.m_fd = fd;
    }
    
    return fd;
}
int cc::Stream::Bind(const EndPoint& ep) const 
{
    return bind(m_fd, ep.address(), ep.address_len());
}
int cc::Stream::Connect(const EndPoint& ep)const 
{
    return connect(m_fd, ep.address(), ep.address_len());
}

void cc::Stream::setNoBlock() const
{
    fcntl(m_fd,O_NONBLOCK);
}

int cc::Stream::CreateTcp(AddressFamily af,cc::Stream& stream)
{
    int fd = socket(af,SOCK_STREAM,IPPROTO_TCP);
    if(fd > 0){
        stream.m_fd = fd;
    }
    return fd;
}

int cc::Stream::CreateUdp(AddressFamily af,cc::Stream& stream)
{
    int fd = socket(af,SOCK_DGRAM,IPPROTO_UDP);
    if(fd > 0){
        stream.m_fd = fd;
    }
    return fd;
}

int &cc::Stream::streamFD()
{
    return m_fd;
}

int cc::EndPoint::resolute(std::string host, int socktype, std::vector<EndPoint> &endpoint)
{
    struct addrinfo hints, *result, *cur;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socktype;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    if (ret == 0)
    {
        cur = result;
        do
        {
            EndPoint ep(cur->ai_addr, cur->ai_addrlen);
            endpoint.push_back(ep);
            cur = cur->ai_next;
        } while (cur != nullptr);
        freeaddrinfo(result);
    }
    else
    {
        std::cout << gai_strerror(ret) << std::endl;
    }
    return ret;
}

cc::EndPoint::EndPoint(const sockaddr *address, socklen_t len)
{
    m_address_len = len;
    this->m_address = (struct sockaddr *)malloc(len);
    std::memset(m_address, 0, len);
    std::memcpy(m_address, address, len);
}

cc::EndPoint::EndPoint(const EndPoint &e)
{
    this->m_address_len = e.m_address_len;
    this->m_address = (struct sockaddr *)malloc(e.m_address_len);
    std::memset(m_address, 0, e.m_address_len);
    std::memcpy(m_address, e.m_address, e.m_address_len);
}
cc::EndPoint::EndPoint(const EndPoint &&e)
{
    this->m_address_len = e.m_address_len;
    this->m_address = (struct sockaddr *)malloc(e.m_address_len);
    std::memset(m_address, 0, e.m_address_len);
    std::memcpy(m_address, e.m_address, e.m_address_len);
}
void cc::EndPoint::operator=(EndPoint & e)
{
    this->m_address_len = e.m_address_len;
    this->m_address = (struct sockaddr *)malloc(e.m_address_len);
    std::memset(m_address, 0, e.m_address_len);
    std::memcpy(m_address, e.m_address, e.m_address_len);
}
void cc::EndPoint::operator=(EndPoint && e)
{
    this->m_address_len = e.m_address_len;
    this->m_address = (struct sockaddr *)malloc(e.m_address_len);
    std::memset(m_address, 0, e.m_address_len);
    std::memcpy(m_address, e.m_address, e.m_address_len);
}
cc::EndPoint::EndPoint(AddressFamily af, std::string ipstr, uint16_t port)
{
    if (af == AF_INET){
        in_addr ip;
        auto c = inet_pton(af,ipstr.c_str(),&ip);
        sockaddr_in enp;
        std::memset(&enp,0,sizeof(enp));
        enp.sin_family = af;
        enp.sin_addr = ip;
        enp.sin_port = htons(port);
        m_address = (sockaddr *)malloc(sizeof(enp));
        std::memcpy(m_address,&enp,sizeof(enp));
        m_address_len = sizeof(enp);
    }else{
        in6_addr ip;
        auto c = inet_pton(af,ipstr.c_str(),&ip);
        sockaddr_in6 enp;
        std::memset(&enp,0,sizeof(enp));
        enp.sin6_family = af;
        enp.sin6_addr = ip;
        enp.sin6_port = htons(port);
        m_address = (sockaddr *)malloc(sizeof(enp));
        std::memcpy(m_address,&enp,sizeof(enp));
        m_address_len = sizeof(enp);
    }
    
}
cc::EndPoint::EndPoint(AddressFamily af, uint16_t port)
{
    if(af == AF_INET){
        sockaddr_in ip;
        ip.sin_addr.s_addr = INADDR_ANY;
        ip.sin_family = af;
        ip.sin_port = htons(port);
        m_address_len = sizeof(ip);
        m_address = (sockaddr *)malloc(m_address_len);
        memcpy(m_address,&ip,m_address_len);
    }else{
        sockaddr_in6 ip;
        ip.sin6_addr = in6addr_any;
        ip.sin6_family = af;
        ip.sin6_flowinfo = 0;
        ip.sin6_port = htons(port);
        m_address_len = sizeof(ip);
        m_address = (sockaddr *)malloc(m_address_len);
        memcpy(m_address,&ip,m_address_len);
    }
}
cc::EndPoint::~EndPoint()
{
    free(m_address);
}

sockaddr *cc::EndPoint::address() const
{
    return m_address;
}

uint16_t cc::EndPoint::port() const
{
    if (m_address->sa_family == AF_INET6)
    {
        struct sockaddr_in6* i6 = (struct sockaddr_in6*)m_address;
        return i6->sin6_port;
    }
    else
    {
        struct sockaddr_in* i4 = (struct sockaddr_in*)m_address;
        return i4->sin_port;
    }
}

void cc::EndPoint::set_port(uint16_t port)
{
    if (m_address->sa_family == AF_INET6)
    {
        struct sockaddr_in6* i6 = (struct sockaddr_in6*)m_address;
        i6->sin6_port = htons(port);
    }
    else
    {
        struct sockaddr_in* i4 = (struct sockaddr_in*)m_address;
        i4->sin_port = htons(port);
    }
}

socklen_t cc::EndPoint::address_len() const
{
    return m_address_len;
}

socklen_t &cc::EndPoint::address_len()
{
    return m_address_len;
}

void cc::EndPoint::info(std::string &ipInfo) const
{
    std::string ip;
    uint16_t port;
    cc::EndPoint::ipAddressAndPort(ip,port);
    ipInfo = std::string("\naddress: ") +
           ip +
           std::string("\nport: ") +
           std::to_string(port);
}

void cc::EndPoint::ipAddressAndPort(std::string &ipAddress, uint16_t &port) const
{
    void *ip = nullptr;
    char ipstr[128];
    if (m_address->sa_family == AF_INET)
    {
        sockaddr_in *ip4 = (sockaddr_in *)(this->m_address);
        port = ip4->sin_port;
        ip = (void *)(&ip4->sin_addr);
    }
    else
    {
        sockaddr_in6 *ip6 = (sockaddr_in6 *)(this->m_address);
        port = ip6->sin6_port;
        ip = (void *)(&ip6->sin6_addr);
    }
    inet_ntop(m_address->sa_family, ip, ipstr, 128);
    ipAddress = std::string(ipstr);
    port = ntohs(port);
}

cc::Block::Block(const void *buffer, const size_t size):m_size(size),m_buffer(malloc(size))
{
    memcpy((void*)m_buffer,buffer,size);
}

cc::Block::~Block()
{
    release();
}

void cc::Block::dealloc()
{
    free((void*)m_buffer);
}

const void *cc::Block::buffer()
{
    return m_buffer;
}

const size_t cc::Block::size()
{
    return m_size;
}
