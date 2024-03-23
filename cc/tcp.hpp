#ifndef TCP_HPP
#define TCP_HPP

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <cstring>
#include <mutex>
#include <unordered_map>

#include "stream.hpp"
#include "select.hpp"
namespace cc
{

    
    class TcpConnectedClient:public virtual Object{   
    public:
        TcpConnectedClient(int socketfd,sockaddr* address);
        virtual ~TcpConnectedClient();
        virtual void dealloc();
        void Close() const;
        void Send(const Block& block) const;
    private:
        void notifyCanSend();
        std::vector<Block> *m_block;
        Stream m_stream;
        sockaddr m_address;
    };

    class TcpServer{

    public:
        TcpServer(AddressFamily m_af);
        ~TcpServer();
        TcpServer(const TcpServer&) = delete;
        TcpServer(const TcpServer&&) = delete;
        void Listen(uint16_t port);
        void recieve();


    private:
        AddressFamily m_af;
        Stream * m_server;
        Select * m_select;
    };
} // namespace cc

#endif