#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/select.h>

#include "cc/poll.hpp"
#include "cc/server.hpp"
#include "cc/connection.hpp"

class Skeleton : public cc::UdpServer::Reciever, public cc::TcpServer::Reciever, public cc::Connection::Delegate

{

public:
    virtual void onRecieve(cc::UdpServer &server, cc::Address &addre, const cc::Block &block)
    {
        server.PrepareSender(addre);
    }
    virtual void onSend(cc::UdpServer &server, cc::Sender &sender)
    {
        cc::Block b("hello");
        sender.Send(b);
        b.Free();
    }
    virtual void onConnect(cc::TcpServer &server, cc::Sender &sender)
    {
        std::cout << sender.address().ipAddress() << " connect" << std::endl;
    }
    virtual void onDisconnect(cc::TcpServer &server, cc::Sender &sender)
    {
        std::cout << sender.address().ipAddress() << " disconnect" << std::endl;
    }
    virtual void onRecieve(cc::TcpServer &server, cc::Sender &sender, const cc::Block &block)
    {
        std::cout << sender.address().ipAddress() << std::endl
                  << block.c_str() << std::endl;
        std::cout << sender.address().ipAddress() << " send someting" << std::endl;

        std::string content = "\r\n\r\n<a href=\"http://localhost:8080/\">abc</a>";
        std::string str = "HTTP/1.1 200 0K \r\n";
        str += "Content-Type: text/html; charset=utf-8 \r\n";
        str += "Content-Length:" + std::to_string(content.size());
        str += content;
        cc::Block b(str);
        server.Prepare(sender.fd(),b);
        b.Free();
    }
    virtual void onSend(cc::TcpServer &server, cc::Sender &sender)
    {
       
    }
    virtual void onError(cc::TcpServer &server, cc::Sender &sender, const char *errmsg)
    {
        std::cout << sender.address().ipAddress() << " " << errmsg << std::endl;
    }
    virtual void onRead(const cc::Connection& connect, cc::Block &block){
        std::cout << block.c_str() << std::endl;
    };
};

int main()
{

    cc::TcpServer us(cc::Domain::ipv4);

    us.SetReciever(new Skeleton());
    int ret = us.Listen(8080);
    if (ret)
    {
        std::cout << strerror(errno) << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cc::Address addr(cc::ipv4, "192.168.2.101", 8080);
    cc::Connection cn(cc::ipv4, cc::stream, cc::tcp, addr);
    cn.SetDelegate(new Skeleton());
    for (size_t i = 0; i < 100; i++)
    {
        cc::Block b(std::to_string(i));
        cn.Send(b);
        b.Free();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    cn.Close();
    return 0;
}