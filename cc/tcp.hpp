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
#include "task.hpp"
namespace cc
{

    class TcpServer;
    class TcpConnectedClientDelegate{
    public:
        virtual void recieve(Block& block) = 0;
    };
    class TcpConnectedClient{   
    public:
        TcpConnectedClient(Stream& socketfd, EndPoint& address,TcpServer* server);
        TcpConnectedClient();
        virtual ~TcpConnectedClient();
        virtual void dealloc();
        void Close() const;
        void Send(const Block& block) const;
        void Recieve();
        void setDelegate(TcpConnectedClientDelegate* delegate); 
        TcpConnectedClientDelegate* delegate(); 
        void notifyCanSend();
    private:
        std::vector<Block> *m_block = nullptr;
        TcpServer* m_server = nullptr; 
        Stream m_stream;
        EndPoint m_address;
        TcpConnectedClientDelegate* m_delegate = nullptr; 
    };

    class TcpServer{

    public:
        TcpServer(AddressFamily m_af);
        ~TcpServer();
        TcpServer(const TcpServer&) = delete;
        TcpServer(const TcpServer&&) = delete;
        void Listen(uint16_t port);
        void removeClient(int fd);
        void addClientPendingWrite(int fd);
        friend class TcpConnectedClient;
    private:
        AddressFamily m_af;
        Stream * m_server = nullptr;
        Select * m_select = nullptr;
        Task *m_task = nullptr;
        bool m_is_running = false;
        std::unordered_map<int,TcpConnectedClient> m_clients;
    };
} // namespace cc

#endif