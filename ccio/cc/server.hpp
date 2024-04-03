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
    class Sender : private Socket
    {
    public:
        Sender(int fd, AddressFamily af, SockType st, Protocol proto, Address &address);
        Sender(AddressFamily af, SockType st, Protocol proto, Address &address);
        Sender();
        void Close() const;
        int Send(const Block &block) const;
        Address address() const;
        int fd() const;

    private:
        Address m_address;
    };
    class UdpServer;

    class UdpServerReciever{
    public:
        virtual void onRecieve(UdpServer &server,Address& addre,const Block& block) {}
        virtual void onSend(UdpServer &server,Sender &sender) {}
    };

    class UdpServer : private Socket
    {
    public:
        UdpServer(AddressFamily af);
        UdpServer(const UdpServer&) = delete;
        UdpServer(const UdpServer&&) = delete;
        int Listen(uint16_t port);
        void Close();
        void SetReciever(UdpServerReciever *reciever);
        void PrepareSender(Address& addre);
    private:
        Loop m_loop;
        Poll m_poll;
        std::mutex m_mutex;
        bool m_is_running = false;
        std::vector<Address> m_addresses;
    };

    class TcpServer;

    class TcpServerReciever{
    public:
        virtual void OnConnect(TcpServer& server,Sender& sender) {}
        virtual void OnDisconnect(TcpServer& server,Sender& sender) {}
        virtual void onRecieve(TcpServer &server,Sender& sender,const Block& block) {}
        virtual void onSend(TcpServer &server,Sender &sender) {}
        virtual void onError(TcpServer &server,Sender& sender,const char* errmsg) {}
    };

    class TcpServer:public Socket{
    public:
        TcpServer(AddressFamily a);
        TcpServer(const TcpServer &) = delete;
        TcpServer(const TcpServer &&) = delete;
        int Listen(uint16_t port);
        void Close();
        void Prepare(int senderfd);
        void SetReciever(TcpServerReciever *reciever);
    private:
        Loop m_loop;
        Poll m_poll;
        std::mutex m_mutex;
        bool m_is_running = false;
        std::unordered_map<int,Sender> m_map_client;
    };
   
} // namespace cc

#endif