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
#include "cc/udp.hpp"
#include "cc/tcp.hpp"
#include "cc/connection.hpp"

class Skeleton : public cc::UdpServer::Reciever, public cc::TcpServer::Reciever, public cc::Connection::Delegate

{

public:
    virtual void onRecieve(cc::UdpServer &server, cc::Address &addre, const cc::Block &block)
    {
        std::cout << block.c_str() << std::endl;
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
        server.Prepare(sender.fd(), b);
        b.Free();
    }
    virtual void onError(cc::TcpServer &server, cc::Sender &sender, const char *errmsg)
    {
        std::cout << sender.address().ipAddress() << " " << errmsg << std::endl;
    }
    virtual void onRead(const cc::Connection &connect, cc::Block &block)
    {
        std::cout << block.c_str() << std::endl;
    };
};

int main()
{

    // cc::TcpServer us(cc::Domain::ipv4);
    Skeleton *skk = new Skeleton();
    // us.SetReciever(skk);
    // int ret = us.Listen(8080);
    // if (ret)
    // {
    //     std::cout << strerror(errno) << std::endl;
    // }
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // cc::Address addr(cc::ipv4, "192.168.2.101", 8080);
    // cc::Connection cn(cc::ipv4, cc::stream, cc::tcp, addr);
    // cn.SetDelegate(new Skeleton());
    // for (size_t i = 0; i < 10; i++)
    // {
    //     cc::Block b(std::to_string(i));
    //     cn.Send(b);
    //     b.Free();
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }
    // cn.Close();

    cc::UdpServer cus(cc::ipv4);
    int ret = cus.Listen(8088);
    cus.SetReciever(skk);
    std::cout << ret<<std::endl;

    // while (true)
    // {
    //     cc::Address ma;
    //     cc::Block b(1024);
    //     cus.RecvFrom(ma,b);
    //     std::cout << b.c_str() << std::endl;
    // }
    
    // for (size_t i = 0; i < 10000; i++)
    // {
    //     cc::Block b(std::to_string(i));
    //     cc::Address addr(cc::ipv4, "192.168.2.101", 9090);
    //     cus.Send(addr,b);
    //     b.Free();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // }
    std::this_thread::sleep_for(std::chrono::seconds(1000));
    return 0;
}