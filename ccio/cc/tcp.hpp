#ifndef TCP_HPP
#define TCP_HPP

#include "stream.hpp"
#include "socket.hpp"
#include "task.hpp"
#include "poll.hpp"
#include <iostream>
#include <string>

namespace cc
{
    class Sender : private Socket
    {
    public:
        Sender(int fd, Domain domain, SockType st, Protocol proto, Address &address);
        Sender(Domain domain, SockType st, Protocol proto, Address &address);
        Sender();
        void Close() const;
        int Send(const Block &block) const;
        Address address() const;
        int fd() const;

    private:
        Address m_address;
    };

    class TcpServer;

    class TcpServer : protected Socket
    {
    public:
        class Reciever
        {
        public:
            virtual void onConnect(TcpServer &server, Sender &sender) {}
            virtual void onDisconnect(TcpServer &server, Sender &sender) {}
            virtual void onRecieve(TcpServer &server, Sender &sender, const Block &block) {}
            virtual void onError(TcpServer &server, Sender &sender, const char *errmsg) {}
        };

    public:
        TcpServer(Domain a);
        TcpServer(const TcpServer &) = delete;
        TcpServer(const TcpServer &&) = delete;
        int Listen(uint16_t port);
        void Close();
        void Prepare(int senderfd, const Block &block);
        void SetReciever(Reciever *reciever);

    private:
        void chectRemoveCache(int senderfd);
        Loop m_loop;
        Poll m_poll;
        std::mutex m_mutex;
        bool m_is_running = false;
        std::unordered_map<int, Sender> m_map_client;
        std::unordered_map<int, std::vector<Block>> m_cache;
    };

} // namespace cc

#endif