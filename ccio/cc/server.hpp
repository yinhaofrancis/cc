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

    class TCPServerDelegate;
    class Client : protected Socket
    {
    public:
        Client(int fd);
        int fd();
        ssize_t Send(const Block &, int flag);
        ssize_t SendTo(const Block &, const Address &address, int flag);
        void Close();
    };
    class TCPServer : protected Socket
    {
    public:
        TCPServer(const TCPServer &) = delete;
        TCPServer(const TCPServer &&) = delete;
        TCPServer(AddressFamily af);
        ~TCPServer();
        void SetDelegate(TCPServerDelegate *delegate);
        void Start(uint16_t port);
        void Stop();
        void WaitClose();
        void PrepareSend(Client &client);

    private:
        bool m_is_running = false;
        Pool *m_pool;
        Poll m_poll;
        std::unordered_map<int, Address> m_map_client;
    };
    class TCPServerDelegate
    {
    public:
        virtual void onConnect(TCPServer &server, Client &fd, Address &address){};
        virtual void onDisconnect(TCPServer &server, Client &fd, const Address &address, const char *msg){};
        virtual void onRead(TCPServer &server, Client &fd, const Address &address, Block &block){};
        virtual void onWrite(TCPServer &server, Client &fd, const Address &address){};
    };
} // namespace cc

#endif