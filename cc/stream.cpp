#include "stream.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

cc::Stream::Stream(int fd) : m_fd(fd) {}

void cc::Stream::Close()
{
    close(m_fd);
}
size_t cc::Stream::Read(char *buffer, size_t size)
{
    return read(m_fd, buffer, size);
}
size_t cc::Stream::Write(char *buffer, size_t size)
{
    return write(m_fd, buffer, size);
}
size_t cc::Stream::Send(char *buffer, size_t size, int flag)
{
    return send(m_fd, buffer, size, flag);
}
size_t cc::Stream::Recv(char *buffer, size_t size, int flag)
{
    return recv(m_fd, buffer, size, flag);
}

size_t cc::Stream::SendTo(char *buffer, size_t size, int flag,const EndPoint& ep)
{
    return sendto(m_fd, buffer, size, flag, ep.address(), ep.address_len());
}
size_t cc::Stream::RecvFrom(char *buffer, size_t size, int flag,EndPoint& ep)
{
    return  recvfrom(m_fd, buffer, size, flag, ep.address(), &ep.address_len());
}
int cc::Stream::Listen(int backlog)
{
    return listen(m_fd, backlog);
}
cc::Stream cc::Stream::Accept(EndPoint& ep)
{
    auto fd = accept(m_fd, ep.address(), &ep.address_len());
    Stream stream(fd);
    return stream;
}
int cc::Stream::Bind(const EndPoint& ep)
{
    return bind(m_fd, ep.address(), ep.address_len());
}
int cc::Stream::Connect(const EndPoint& ep)
{
    return connect(m_fd, ep.address(), ep.address_len());
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
cc::EndPoint::EndPoint(int af, std::string ipstr, uint16_t port)
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

std::string cc::EndPoint::info()
{
    void *ip = nullptr;
    char ipstr[128];
    int16_t port;
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
    return std::string("\naddress: ") +
           std::string(ipstr) +
           std::string("\nport: ") +
           std::to_string(ntohs(port));
}
