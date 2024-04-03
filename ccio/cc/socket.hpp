#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "stream.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace cc
{

    enum AddressFamily
    {
        ipv4 = AF_INET,
        ipv6 = AF_INET6
    };
    enum SockType
    {
        stream  = SOCK_STREAM,
        dgram   = SOCK_DGRAM
    };
    enum Protocol
    {
        tcp     = IPPROTO_TCP,
        udp     = IPPROTO_UDP
    };
    class Address: public cc::Ref
    {
    public:
        Address(AddressFamily af, const char *ip, uint16_t port);
        Address(AddressFamily af, uint16_t port);
        Address();
        AddressFamily family() const;
        uint16_t port() const;
        void modifyAddress(AddressFamily af, const char *ip,uint16_t port);
        std::string ipAddress() const;
        friend class Socket;
    private:
        sockaddr *m_address = nullptr;
        socklen_t m_size = 0;
       
    };

    class Socket : public Stream
    {
    public:
        Socket(AddressFamily af,SockType st,Protocol proto);
        Socket(int fd);
        ssize_t Send(const Block &, int flag) const;
        ssize_t Recieve(Block &, int flag) const;
        ssize_t SendTo(const Block &, const Address &address, int flag) const;
        ssize_t RecvFrom(Block &, Address &address, int flag) const;
        int Bind(Address &address) const;
        int Listen(int backlog) const;
        int Accept(Address &address) const;
        int Connect(const Address &address) const;
        AddressFamily af(); 
        static Socket createTCP(AddressFamily af);
        static Socket createUDP(AddressFamily af);

    protected:
        AddressFamily m_af;
        SockType m_st;
        Protocol m_proto;
    };

} // namespace cc

#endif