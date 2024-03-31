#include "socket.hpp"
#include <cstring>
#include <string>


cc::Socket::Socket(AddressFamily af, SockType st, Protocol proto)
:Stream(socket(af,st,proto)),m_af(af),m_st(st),m_proto(proto)
{}

cc::Socket::Socket(int fd):Stream(fd)
{
}

ssize_t cc::Socket::Send(const Block &block, int flag)
{
    return send(this->fd(), block.data(), block.size(), flag);
}

ssize_t cc::Socket::Recieve(Block &block, int flag)
{
    ssize_t siz = recv(fd(), const_cast<void *>(block.data()), block.size(), flag);
    block.m_size = siz;
    return siz;
}

ssize_t cc::Socket::SendTo(const Block &block, const Address &address, int flag)
{
    return sendto(fd(), block.data(), block.size(), flag, address.m_address, address.m_size);
}

ssize_t cc::Socket::RecvFrom(Block &block, Address &address, int flag)
{
    return recvfrom(fd(), const_cast<void *>(block.data()), block.size(), flag, address.m_address, &address.m_size);
}

int cc::Socket::Bind(Address &address)
{
    return bind(fd(), address.m_address, address.m_size);
}

int cc::Socket::Listen(int backlog)
{
    return listen(fd(), backlog);
}

int cc::Socket::Accept(Address &address)
{
    return accept(fd(), address.m_address, &address.m_size);
}

cc::Socket cc::Socket::createTCP(AddressFamily af)
{
    return Socket(af,stream,tcp);
}

cc::Socket cc::Socket::createUDP(AddressFamily af)
{
    return Socket(af,dgram,udp);
}

cc::AddressFamily cc::Address::family()
{
    return (AddressFamily)m_address->sa_family;
}

uint16_t cc::Address::port()
{
    switch (family())
    {
    case ipv4:
        return htons(reinterpret_cast<sockaddr_in *>(m_address)->sin_port);
    default:
        return htons(reinterpret_cast<sockaddr_in6 *>(m_address)->sin6_port);
    }
}

cc::Address::Address(AddressFamily af, const char *ip, uint16_t port)
{
    modifyAddress(af, ip, port);

}

void cc::Address::modifyAddress(AddressFamily af, const char *ip, uint16_t port)
{
    uint16_t nPort = htons(port);
    switch (af)
    {
    case ipv4:
    {
        in_addr address;
        inet_pton(af, ip, &address);
        sockaddr_in saddr;
        saddr.sin_family = af;
        saddr.sin_port = nPort;
        saddr.sin_addr = address;
        m_size = sizeof(sockaddr_in);
        if(this->m_address != nullptr){
            free(this->m_address);
        }
        this->m_address = (sockaddr *)malloc(m_size);
        std::memcpy(this->m_address, &saddr, m_size);
    }
    break;
    default:
    {
        in6_addr address;
        inet_pton(af, ip, &address);
        sockaddr_in6 saddr;
        saddr.sin6_addr = address;
        saddr.sin6_family = af;
        saddr.sin6_port = nPort;
        saddr.sin6_flowinfo = 0;
        saddr.sin6_scope_id = 0;
        m_size = sizeof(sockaddr_in6);
        if(this->m_address != nullptr){
            free(this->m_address);
        }
        this->m_address = (sockaddr *)malloc(m_size);
        std::memcpy(this->m_address, &saddr, m_size);
    }
    break;
    }
    this->dealloc([this](){
        if(this->m_address == nullptr){
            free(this->m_address);
        }
    });
}

cc::Address::Address(AddressFamily af, uint16_t port)
{
    switch (af)
    {
    case ipv4:
        modifyAddress(af,"0.0.0.0",port);
        break;
    case ipv6:
        modifyAddress(af,"::",port);
    }
    
}

cc::Address::Address():Address(cc::ipv4,0)
{}

std::string cc::Address::ipAddress()
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
