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

int cc::EndPoint::resolute(std::string host, int16_t port, int socktype, std::vector<EndPoint> &endpoint)
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
            EndPoint ep(cur->ai_addr, cur->ai_addrlen, port);
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

cc::EndPoint::EndPoint(const sockaddr *address, socklen_t len, int16_t port)
{
    memcpy(&m_address, address, len);
    m_port = port;
}

const sockaddr &cc::EndPoint::address()
{
    return m_address;
}
int16_t &cc::EndPoint::port()
{
    return m_port;
}
