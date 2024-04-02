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

    class ServerTest : protected Socket
    {
    public:
        class Sender : protected Socket
        {
        public:
            Sender(int fd, ServerTest &server, const Address &address, Protocol &protocol);
            int fd();
            ssize_t Send(const Block &, int flag);
            void Close();

        protected:
            Address m_address;
            Protocol m_proto;
            ServerTest &m_server;
        };

    public:
        ServerTest(const ServerTest &) = delete;
        ServerTest(const ServerTest &&) = delete;
        ServerTest(AddressFamily af, SockType sock, Protocol proto);
        ~ServerTest();
        void SetDelegate(ServerDelegate *delegate);

        int Start(uint16_t port);
        void Stop();
        void WaitClose();
        void PrepareSend(int);
        Sender ServerSender(Address &to);
        void RemoveSender(Sender &sender);

    private:
        bool m_is_running = false;
        Pool *m_pool = nullptr;
        Poll m_poll;
        std::unordered_map<int, Address> m_map_client;
        void tcp_server_core_process(cc::ServerDelegate *delegate);
        void udp_server_core_process(cc::ServerDelegate *delegate);
    };
    class ServerDelegate
    {
    public:
        virtual void onConnect(ServerTest &server, ServerTest::Sender &fd, Address &address){};
        virtual void onDisconnect(ServerTest &server, ServerTest::Sender &fd, const Address &address, const char *msg){};
        virtual void onRead(ServerTest &server, ServerTest::Sender &fd, const Address &address, Block &block){};
        virtual void onWrite(ServerTest &server, ServerTest::Sender &fd, const Address &address){};
    };

    
} // namespace cc

#endif