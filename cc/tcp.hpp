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

#include "stream.hpp"


namespace cc
{
    class TcpServer;
    class TcpConnectedClient;

    class TcpServerDelegate
    {
    public:
        virtual void acceptClient(const TcpServer& server,Stream& stream,EndPoint& point) = 0;
    };
    class TcpServer
    {
    public:
        TcpServer(AddressFamily af, TcpServerDelegate* delegate);
        ~TcpServer();
        void Listen(uint16_t port);
    private:
        TcpServerDelegate* m_delegate = nullptr;
        Stream* m_stream;
        AddressFamily m_af;
        bool m_is_running;
    };

    class TcpConnectedClientDelegate
    {
    public:
        virtual bool recieveClient(const TcpConnectedClient& client,const void* buffer,size_t size) = 0;
    };
    class TcpConnectedClient{
    public:
        TcpConnectedClient(Stream& stream,EndPoint& point);
        ~TcpConnectedClient();
        TcpConnectedClient(TcpConnectedClient&) = delete;
        TcpConnectedClient(TcpConnectedClient&&) = delete;
        void SendBuffer(const void* buffer,const size_t size) const;
        void SendString(const std::string buffer) const;
        void SetDelegate(TcpConnectedClientDelegate* delegate);
        void Close();
    private:
        struct PendingBuffer{
            const void* buffer;
            size_t size;
        };
        Stream *m_stream;
        EndPoint m_endpoint;
        bool m_is_running;
        TcpConnectedClientDelegate* m_delegate;
        std::vector<PendingBuffer> *m_pendding_buffer;

    };
} // namespace cc

#endif