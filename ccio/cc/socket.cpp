#include "socket.hpp"
#include <cstring>
#include <string>

cc::Socket::Socket(Domain domain, SockType st, Protocol proto)
    : Stream(socket(domain, st, proto)), m_protocol(proto), m_domain(domain), m_socktype(st)
{
}

cc::Socket::Socket(int fd) : Stream(fd)
{
}

ssize_t cc::Socket::Send(const Block &block, int flag) const
{
    return send(this->fd(), block.data(), block.size(), flag);
}

ssize_t cc::Socket::Recieve(Block &block, int flag) const
{
    ssize_t siz = recv(fd(), const_cast<void *>(block.data()), block.size(), flag);
    block.m_size = siz;
    return siz;
}

ssize_t cc::Socket::SendTo(const Block &block, const Address &address, int flag) const
{
    return sendto(fd(), block.data(), block.size(), flag, address.m_address, address.m_size);
}

ssize_t cc::Socket::RecvFrom(Block &block, Address &address, int flag) const
{
    return recvfrom(fd(), const_cast<void *>(block.data()), block.size(), flag, address.m_address, &address.m_size);
}

int cc::Socket::Bind(Address &address) const
{
    return bind(fd(), address.m_address, address.m_size);
}

int cc::Socket::Listen(int backlog) const
{
    return listen(fd(), backlog);
}

int cc::Socket::Accept(Address &address) const
{
    return accept(fd(), address.m_address, &address.m_size);
}

int cc::Socket::Connect(const Address &address) const
{
    return connect(fd(), address.m_address, address.m_size);
}

cc::Socket cc::Socket::createTCP(Domain domain)
{
    return Socket(domain, stream, tcp);
}

cc::Socket cc::Socket::createUDP(Domain domain)
{
    return Socket(domain, dgram, udp);
}

cc::SockType cc::Socket::sockType() const
{
    return m_socktype;
}

cc::Protocol cc::Socket::protocol() const
{
    return m_protocol;
}

void cc::Socket::setSockOptSockLevelInt(SocketOption so, int flag)
{
    socklen_t size;
    setsockopt(fd(), SOL_SOCKET, so, &flag, sizeof(int));
}

int cc::Socket::getSockOptSockLevelInt(SocketOption so)
{
    int value = 0;
    socklen_t size;
    getsockopt(fd(), SOL_SOCKET, so, &value, &size);
    return value;
}

int cc::Socket::error() const
{
    int value = 0;
    socklen_t size;
    getsockopt(fd(), SOL_SOCKET, SO_ERROR, &value, &size);
    return value;
}

void cc::Socket::setReuseAddr(bool flag)
{
    setSockOptSockLevelInt(ReuseAddr, flag);
}

void cc::Socket::setReusePort(bool flag)
{
    setSockOptSockLevelInt(ReusePort, flag);
}

void cc::Socket::setBroadCast(bool flag)
{
    setSockOptSockLevelInt(BroadCast, flag);
}

cc::Domain cc::Socket::domain() const
{
    return m_domain;
}

cc::Domain cc::Address::family() const
{
    return (Domain)m_address->sa_family;
}

uint16_t cc::Address::port() const
{
    switch (family())
    {
    case ipv4:
        return htons(reinterpret_cast<sockaddr_in *>(m_address)->sin_port);
    default:
        return htons(reinterpret_cast<sockaddr_in6 *>(m_address)->sin6_port);
    }
}

cc::Address::Address(Domain domain, const char *ip, uint16_t port)
{
    modifyAddress(domain, ip, port);
}

void cc::Address::modifyAddress(Domain domain, const char *ip, uint16_t port)
{
    uint16_t nPort = htons(port);
    switch (domain)
    {
    case ipv4:
    {
        in_addr address;
        inet_pton(domain, ip, &address);
        sockaddr_in saddr;
        saddr.sin_family = domain;
        saddr.sin_port = nPort;
        saddr.sin_addr = address;
        m_size = sizeof(sockaddr_in);
        if (this->m_address != nullptr)
        {
            free(this->m_address);
        }
        this->m_address = (sockaddr *)malloc(m_size);
        std::memcpy(this->m_address, &saddr, m_size);
    }
    break;
    default:
    {
        in6_addr address;
        inet_pton(domain, ip, &address);
        sockaddr_in6 saddr;
        saddr.sin6_addr = address;
        saddr.sin6_family = domain;
        saddr.sin6_port = nPort;
        saddr.sin6_flowinfo = 0;
        saddr.sin6_scope_id = 0;
        m_size = sizeof(sockaddr_in6);
        if (this->m_address != nullptr)
        {
            free(this->m_address);
        }
        this->m_address = (sockaddr *)malloc(m_size);
        std::memcpy(this->m_address, &saddr, m_size);
    }
    break;
    }
}

cc::Address::Address(Domain domain, uint16_t port)
{
    switch (domain)
    {
    case ipv4:
        modifyAddress(domain, "0.0.0.0", port);
        break;
    case ipv6:
        modifyAddress(domain, "::", port);
    }
}

cc::Address::Address(const Address &a)
{
    copy(a);
}
void cc::Address::copy(const cc::Address &a)
{
    if (m_address != nullptr && m_size != a.m_size)
    {
        m_address = (sockaddr *)realloc(m_address,a.m_size);
    }else if(m_address == nullptr){
        m_address = (sockaddr *)malloc(a.m_size);
    }
    this->m_size = a.m_size;
    memcpy(this->m_address, a.m_address, m_size);
}
cc::Address::Address(const Address &&a)
{
    copy(a);
}

void cc::Address::operator=(const Address &a)
{
    copy(a);
}
void cc::Address::operator=(const Address &&a)
{
    copy(a);
}
cc::Address::Address() : Address(cc::ipv4, 0)
{
}

std::string cc::Address::ipAddress() const
{
    char result[256];
    std::memset(result, 0, sizeof(result));
    switch (family())
    {
    case ipv4:
    {
        auto address = reinterpret_cast<sockaddr_in *>(this->m_address)->sin_addr;
        inet_ntop(family(), &address, result, sizeof(result));
        return result;
    }

    case ipv6:
    {
        auto address6 = reinterpret_cast<sockaddr_in6 *>(this->m_address)->sin6_addr;
        inet_ntop(family(), &address6, result, sizeof(result));
        return result;
    }
    }
}
