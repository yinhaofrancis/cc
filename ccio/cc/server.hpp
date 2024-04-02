#ifndef SERVER_HPP
#define SERVER_HPP
#include "stream.hpp"
#include "socket.hpp"
#include "task.hpp"
#include "poll.hpp"
#include <iostream>
#include <string>

namespace cc
{

    class ServerDelegate;
    class Server : protected Socket
    {
    public:
        class Client : protected Socket
        {
        public:
            Client(int fd, const Address &address, Protocol &protocol);
            int fd();
            ssize_t Send(const Block &, int flag);
            void Close();

        protected:
            Address m_address;
            Protocol m_proto;
        };

    public:
        Server(const Server &) = delete;
        Server(const Server &&) = delete;
        Server(AddressFamily af, SockType sock, Protocol proto);
        ~Server();
        void SetDelegate(ServerDelegate *delegate);

        int Start(uint16_t port);
        void Stop();
        void WaitClose();
        void PrepareSend(int);
        Client PrepareSendTo(Address &address);

    private:
        bool m_is_running = false;
        Pool *m_pool;
        Poll m_poll;
        std::unordered_map<int, Address> m_map_client;
        void tcp_server_core_process(cc::ServerDelegate *delegate);
        void udp_server_core_process(cc::ServerDelegate *delegate);
    };
    class ServerDelegate
    {
    public:
        virtual void onConnect(Server &server, Server::Client &fd, Address &address){};
        virtual void onDisconnect(Server &server, Server::Client &fd, const Address &address, const char *msg){};
        virtual void onRead(Server &server, Server::Client &fd, const Address &address, Block &block){};
        virtual void onWrite(Server &server, Server::Client &fd, const Address &address){};
    };
} // namespace cc

#endif