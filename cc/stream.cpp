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

size_t cc::Stream::SendTo(char *buffer, size_t size, int flag, const struct sockaddr *address, socklen_t addrlen)
{
    return sendto(m_fd, buffer, size, flag, address, addrlen);
}
size_t cc::Stream::RecvFrom(char *buffer, size_t size, int flag, struct sockaddr *address, socklen_t *addrlen)
{
    return recvfrom(m_fd, buffer, size, flag, address, addrlen);
}
int cc::Stream::Listen(int backlog)
{
    return listen(m_fd, backlog);
}
cc::Stream cc::Stream::Accept(struct sockaddr *address, socklen_t *addrlen)
{
    auto fd = accept(m_fd, address, addrlen);
    Stream stream(fd);
    return stream;
}
int cc::Stream::Bind(const struct sockaddr *address, socklen_t addrlen)
{
    return bind(m_fd, address, addrlen);
}
int cc::Stream::Connect(const struct sockaddr *address, socklen_t addrlen)
{
    return connect(m_fd, address, addrlen);
}

int &cc::Stream::streamFD()
{
    return m_fd;
}

int cc::EndPoint::resolute(std::string host, int socktype, std::vector<EndPoint> &endpoint)
{
    struct addrinfo hints, *result, *cur;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socktype;
    hints.ai_flags = AI_PASSIVE;

    int ret = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    if (ret == 0)
    {
        cur = result;
        do
        {
            EndPoint ep(cur->ai_addr, cur->ai_addrlen, cur->ai_family);
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

cc::EndPoint::EndPoint(const sockaddr *address, socklen_t len, sa_family_t family) : m_ai_family(family)
{
    memset(&m_address, 0, sizeof(sockaddr));
    memcpy(&m_address, address, len);
    m_addrlen = len;
    m_ai_family = family;
}

const sockaddr &cc::EndPoint::address()
{
    return m_address;
}

std::string cc::EndPoint::info()
{
    void* ip = nullptr;
    char ipstr[128];
    int16_t port;
    if (m_ai_family == AF_INET)
    {
        sockaddr_in *ip4 = (sockaddr_in *)(&this->m_address);
        port = ip4->sin_port;
        ip = (void*)(&ip4->sin_addr);
    }
    else
    {
        sockaddr_in6 *ip6 = (sockaddr_in6 *)(&this->m_address);
        port = ip6->sin6_port;
        ip = (void*)(&ip6->sin6_addr);
    }
    inet_ntop(this->m_ai_family, ip, ipstr, 128);
    return std::string("\naddress: ") +
               std::string(ipstr) +
               std::string("\nport: ") +
               std::to_string(ntohs(port));
}
