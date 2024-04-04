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
    
    class UdpServer;

    class UdpServer : protected Socket
    {
    public:
        class Reciever
        {
        public:
            virtual void onRecieve(UdpServer &server, Address &addre, const Block &block) {}
        };

    public:
        UdpServer(Domain domain);
        UdpServer(const UdpServer &) = delete;
        UdpServer(const UdpServer &&) = delete;
        int Listen(uint16_t port);
        void Close();
        void SetReciever(Reciever *reciever);
        void Send(Address &addre,const Block&);
        void RecvFrom(Address &addre,Block&);

    private:
        struct Message{
            Address addre;
            Block block;
            Message();
        };
        void udpLoop();
        Reciever *m_reciever = nullptr;
        Loop m_loop;
        Poll m_poll;
        std::mutex m_mutex;
        bool m_is_running = false;
        std::vector<Message> m_message;
    };

    

} // namespace cc

#endif