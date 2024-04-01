#ifndef SERVER_HPP
#define SERVER_HPP
#include "stream.hpp"
#include "socket.hpp"
#include "task.hpp"
#include "poll.hpp"
#include<iostream>
#include<string>    

namespace cc
{

    class TCPServerDelegate;
    class Client:protected Socket{
    public:
        Client(int fd);
        int fd();
        ssize_t Send(const Block &, int flag);
        ssize_t SendTo(const Block &, const Address &address, int flag);
        void Close();
    };
    class TCPServer:protected Socket
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
        void PrepareSend(Client& client);
    private:
        bool m_is_running = false;
        Pool * m_pool;
        Poll m_poll;
        std::unordered_map<int,Client> m_map_client;
    };
    class TCPServerDelegate{
    public:
        virtual void onConnect(TCPServer& server, Client &fd,Address& address) {
            std::cout << "onConnect:" << address.ipAddress() << std::endl;
            server.PrepareSend(fd);
        };
        virtual void onDisconnect(TCPServer& server,Client &fd,const char* msg) {
            if(msg != nullptr){
                std::cout << "onDisconnect:" << msg << std::endl;
            }else{
                std::cout << "onDisconnect"  << std::endl;
            }
            
        };
        virtual void onRead(TCPServer& server,Client &fd,Block& block) {
            std::cout << "onRead:" << block.c_str() << std::endl;
            server.PrepareSend(fd);
        };
        virtual void onWrite(TCPServer& server,Client &fd) {
            std::cout << "onWrite"<< std::endl;
            Block b("hello");
            fd.Send(b,0);
        };
    };
} // namespace cc








#endif