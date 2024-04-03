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
    enum OptionLevel
    {
        socketLevel = SOL_SOCKET,
        tcpLevel = IPPROTO_TCP,
        udplevel = IPPROTO_UDP
    };
    enum SocketOption
    {
        ReuseAddr = SO_REUSEADDR,
        Error = SO_ERROR,
        BroadCast = SO_BROADCAST,
        SndBuf = SO_SNDBUF,
        RcvBuf = SO_RCVBUF,
        KeepLive = SO_KEEPALIVE,
        OobInline = SO_OOBINLINE,
        NoCheck = SO_NO_CHECK,
        Priority = SO_PRIORITY,
        Linger = SO_LINGER,
        BsdCompat = SO_BSDCOMPAT,
        ReusePort = SO_REUSEPORT,
        PassCred = SO_PASSCRED,
        PeerCred = SO_PEERCRED,
        RvcLowat = SO_RCVLOWAT,
        SndLowat = SO_SNDLOWAT,
        RcvTimeOut = SO_RCVTIMEO,
        SndTimeOut = SO_SNDTIMEO,
        BindToDevice = SO_BINDTODEVICE,
    };
    enum Domain
    {
        ipv4 = AF_INET,
        ipv6 = AF_INET6
    };
    enum SockType
    {
        stream = SOCK_STREAM,
        dgram = SOCK_DGRAM
    };
    enum Protocol
    {
        tcp = IPPROTO_TCP,
        udp = IPPROTO_UDP
    };
    class Address : public cc::Ref
    {
    public:
        Address(Domain domain, const char *ip, uint16_t port);
        Address(Domain domain, uint16_t port);
        Address();
        Domain family() const;
        uint16_t port() const;
        void modifyAddress(Domain domain, const char *ip, uint16_t port);
        std::string ipAddress() const;
        friend class Socket;
    private:
        sockaddr *m_address = nullptr;
        socklen_t m_size = 0;
    };

    class Socket : public Stream
    {
    public:
        Socket(Domain domain, SockType st, Protocol proto);
        Socket(int fd);
        ssize_t Send(const Block &, int flag) const;
        ssize_t Recieve(Block &, int flag) const;
        ssize_t SendTo(const Block &, const Address &address, int flag) const;
        ssize_t RecvFrom(Block &, Address &address, int flag) const;
        int Bind(Address &address) const;
        int Listen(int backlog) const;
        int Accept(Address &address) const;
        int Connect(const Address &address) const;
        Domain domain() const;
        static Socket createTCP(Domain domain);
        static Socket createUDP(Domain domain);
        SockType sockType() const;
        Protocol protocol() const;
        int error() const;
    };

} // namespace cc

#endif