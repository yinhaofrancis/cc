#ifndef define_hpp
#define define_hpp
#include <iostream>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <fcntl.h>
#include <sys/un.h>
#include <string>
#include <cstring>

namespace ipc
{
    enum domain
    {

        ipv4 = AF_INET,

        ipv6 = AF_INET6,

        local = AF_UNIX,

        unspec = AF_UNSPEC,

    };

    enum sock
    {

        strm = SOCK_STREAM,

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

        none = 0
    };

    enum status
    {
        nonblock = O_NONBLOCK,

        rd_only = O_RDONLY,

        wr_only = O_WRONLY,

        append = O_APPEND,

        creat = O_CREAT,

        rw = O_RDWR,

        trunc = O_TRUNC,

        sync = O_SYNC
    };
    status operator|(status v1, status v2);

    template <domain d>
    class address
    {
    public:
        address(const char *path)
        {
            static_assert(d == unix,"domain is not unix");
            static_assert(sizeof(path) < 104,"path len too large");
            memset(buffer, 0, sizeof(buffer));
            sockaddr_un *addr = reinterpret_cast<sockaddr_un *>(buffer);
            addr->sun_family = AF_UNIX;
            strcpy(addr->sun_path,path);
            #if __APPLE || __UNIX__
            addr->sun_len = strlen(path);
            #endif
        }
        address(const char *ip, uint16_t port)
        {
            static_assert(d == ipv4 || d == ipv6, "domain is not ipv4 or ipv6");
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

        std::string path(){
            static_assert(d == unix , "domain is not unix");
            if (d == unix){
                sockaddr_un *addr = reinterpret_cast<sockaddr_un *>(buffer);
                return std::string(addr->sun_path);
            }
        }
        uint16_t port()
        {
            static_assert(d == ipv4 || d == ipv6, "domain is not ipv4 or ipv6");
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
            static_assert(d == ipv4 || d == ipv6, "domain is not ipv4 or ipv6");
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
            else if (d == unix){
                std::cout << SUN_LEN((sockaddr_un*)this->raw()) << std::endl << sizeof(sockaddr_un) << std::endl;
                return sizeof(sockaddr_un);
            }
            return 0;
        }

    private:
        uint8_t buffer[256];
    };
} // namespace ipc

#endif