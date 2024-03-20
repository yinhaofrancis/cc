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


    class TcpServer
    {
    public:
        struct Client
        {
            Stream stream;
            EndPoint ep = EndPoint(0,0);
            bool isInvalid = false;
        };
        struct Context{
            Select select;
            Select clientSelect;
            AddressFamily af;
            Stream stream;
            bool m_runing = false;
            std::unordered_map<int,Client> clients;
            std::mutex lock;
        };
        class Delegate{
        public:
            virtual void recieve(const TcpServer::Client&,void* buffer, size_t size) = 0;
            virtual ~Delegate() = 0;
        };
    public:

        TcpServer(AddressFamily af);
        ~TcpServer();
        void Listen(uint16_t port,Delegate *delegate);
        bool isRunning();
    private:
        Context* m_ctx = nullptr;
        AddressFamily m_af;
        void RecieveDataProcess(cc::TcpServer::Context *c_ctx, void *buffer, Delegate *delegate);
        void ErrorStreamProcess(cc::TcpServer::Context *c_ctx);
    };
} // namespace cc

#endif