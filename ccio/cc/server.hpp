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

    // class Server;
    // class Sender;
    // class Reciever
    // {
    // public
    //     //链接
    //     virtual void onConnect(Server &server,Sender& sender){}
    //     //链接断开
    //     virtual void onDisconnect(Server &server,Sender& sender){}
    //     //收到dgram data
    //     virtual void onRecieve(Server &server,Sender& sender,const Block& block) {}
    //     //收到数据
    //     virtual void onRead(Server &server, std::vector<Block> &block){}
    //     //收到挂断
    //     virtual void onHungUp(Server &server,Sender& sender){}
    //     //收到错误
    //     virtual void onError(Server &server,Sender&, const std::string &errormsg){}
    // };
    class Sender : private Socket
    {
    public:
        Sender(int fd, AddressFamily af, SockType st, Protocol proto, Address &address);
        Sender(AddressFamily af, SockType st, Protocol proto, Address &address);
        Sender();
        void Close() const;
        int Send(const Block &block) const;
        Address address() const;

    private:
        Address m_address;
    };
    class UdpServer;

    class UdpServerReciever{
        virtual void onRecieve(UdpServer &server,Address& addre,const Block& block) {}
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
    private:
        Loop m_loop;
        Poll m_poll;
        bool m_is_running = false;
    };
    // class Server:private Socket
    // {
    // public:
    //     Server(AddressFamily af, SockType st, Protocol proto);
    //     Server(const Server&) = delete;
    //     Server(const Server&&) = delete;
    //     ~Server();
    //     int Listen(uint16_t port);
    //     int Accept();
    //     int Wait();
    //     int Close();
    //     void setReciever(Reciever* reciever);
    // private:
    //     std::unordered_map<int,Sender> m_map_sender;
    //     bool m_is_running = false;
    //     Loop *m_loop  = nullptr;
    //     Poll m_poll;
    // };
    // class ServerDelegate;

    // class ServerTest : protected Socket
    // {
    // public:
    //     class Sender : protected Socket
    //     {
    //     public:
    //         Sender(int fd, ServerTest &server, const Address &address, Protocol &protocol);
    //         int fd();
    //         ssize_t Send(const Block &, int flag);
    //         void Close();

    //     protected:
    //         Address m_address;
    //         Protocol m_proto;
    //         ServerTest &m_server;
    //     };

    // public:
    //     ServerTest(const ServerTest &) = delete;
    //     ServerTest(const ServerTest &&) = delete;
    //     ServerTest(AddressFamily af, SockType sock, Protocol proto);
    //     ~ServerTest();
    //     void SetDelegate(ServerDelegate *delegate);

    //     int Start(uint16_t port);
    //     void Stop();
    //     void WaitClose();
    //     void PrepareSend(int);
    //     Sender ServerSender(Address &to);
    //     void RemoveSender(Sender &sender);

    // private:
    //     bool m_is_running = false;
    //     Pool *m_pool = nullptr;
    //     Poll m_poll;
    //     std::unordered_map<int, Address> m_map_client;
    //     void tcp_server_core_process(cc::ServerDelegate *delegate);
    //     void udp_server_core_process(cc::ServerDelegate *delegate);
    // };
    // class ServerDelegate
    // {
    // public:
    //     virtual void onConnect(ServerTest &server, ServerTest::Sender &fd, Address &address){};
    //     virtual void onDisconnect(ServerTest &server, ServerTest::Sender &fd, const Address &address, const char *msg){};
    //     virtual void onRead(ServerTest &server, ServerTest::Sender &fd, const Address &address, Block &block){};
    //     virtual void onWrite(ServerTest &server, ServerTest::Sender &fd, const Address &address){};
    // };

    // class Server{
    // public:
    //     class Delegate{
    //     public:
    //         virtual void onRead(Server &server,std::vector<Block> &block){};
    //         virtual void onWrite(Server &server){};
    //         virtual void onHungUp(Server &server){};
    //         virtual void onError(Server &server,const std::string& errormsg){};
    //     };
    // public:
    //     Server(Socket &socket);
    //     ~Server();
    //     Server(const Server &) = delete;
    //     Server(const Server &&) = delete;
    //     int Listen(uint16_t port);
    //     void setDelegate(Delegate * delegate);
    // private:
    //     Socket m_socket;
    //     Poll m_poll;
    //     Loop m_loop;
    //     bool m_is_running = false;
    // };

} // namespace cc

#endif