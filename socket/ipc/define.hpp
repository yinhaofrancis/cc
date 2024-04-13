#ifndef define_hpp
#define define_hpp

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <fcntl.h>

namespace ipc
{
    enum domain
    {

        ipv4 = AF_INET,

        ipv6 = AF_INET6,

        unix = AF_UNIX,

        unspec = AF_UNSPEC,

    };

    enum sock
    {

        stream = SOCK_STREAM,

        dgram = SOCK_DGRAM,

        raw = SOCK_RAW,

        rdm = SOCK_RDM,

        seqpacket = SOCK_SEQPACKET,
    };

    enum protocol
    {

        icmp = IPPROTO_ICMP,

        icmpv6 = IPPROTO_ICMPV6,

        tcp = IPPROTO_TCP,

        udp = IPPROTO_UDP,

        ip = IPPROTO_IP,

        ah = IPPROTO_AH,

        esp = IPPROTO_ESP,

    };

    enum status
    {
        nonblock = O_NONBLOCK,

        rd_only = O_RDONLY,

        wr_only = O_WRONLY,

        append = O_APPEND,

        creat = O_CREAT,

        trunc = O_TRUNC,

        sync = O_SYNC
    };

   

    template <domain d>
    class address
    {
    public:
        address(const char *ip, uint16_t port)
        {
            memset(buffer, 0, sizeof(buffer));
            if (d == ipv4)
            {
                sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(buffer);
                addr->sin_family = AF_INET;
                addr->sin_port = htons(port);
                inet_pton(AF_INET, ip, &addr->sin_addr);
            }
            else if (d == ipv6)
            {
                sockaddr_in6 *addr = reinterpret_cast<sockaddr_in6 *>(buffer);
                addr->sin6_family = AF_INET;
                addr->sin6_port = htons(port);
                inet_pton(AF_INET6, ip, &addr->sin6_addr);
                addr->sin6_scope_id = 0;
                addr->sin6_flowinfo = 0;
            }
        }

        const domain m_domain = d;

        uint16_t port()
        {
            if (d == ipv4)
            {
                sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(buffer);
                return ntohs(addr->sin_port);
            }
            else if (d == ipv6)
            {
                sockaddr_in6 *addr = reinterpret_cast<sockaddr_in6 *>(buffer);
                return ntohs(addr->sin6_port);
            }
            return 0;
        }
        std::string ip_address()
        {
            char result[256];
            std::memset(result, 0, sizeof(result));
            if (d == ipv4)
            {
                sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(buffer);
                inet_ntop(m_domain, &addr->sin_addr, result, sizeof(result));
            }
            else if (d == ipv6)
            {
                sockaddr_in6 *addr = reinterpret_cast<sockaddr_in6 *>(buffer);
                inet_ntop(m_domain, &addr->sin6_addr, result, sizeof(result));
            }
            return std::string(result);
        }
        const sockaddr *raw()
        {
            return reinterpret_cast<sockaddr *>(buffer);
        }
        const size_t size()
        {
            if (d == ipv4)
            {
                return sizeof(sockaddr_in);
            }
            else if (d == ipv6)
            {
                return sizeof(sockaddr_in6);
            }
            return 0;
        }

    private:
        uint8_t buffer[sizeof(sockaddr_in6)];
    };
} // namespace ipc

#endif