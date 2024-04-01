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

#include "cc/ref.hpp"
#include "cc/block.hpp"
#include "cc/stream.hpp"
#include "cc/socket.hpp"
#include "cc/task.hpp"
#include "cc/poll.hpp"
#include "main.h"
#include "cc/server.hpp"
void tudp()
{
    auto udp = cc::Socket::createUDP(cc::ipv4);
    auto localhost = cc::Address(cc::ipv4, 8080);
    udp.Bind(localhost);
    udp.Listen(256);
    cc::Poll poll;
    poll.add(udp.fd(), cc::Poll::IN);
    while (true)
    {
        std::vector<cc::Poll::Result> ret;
        int i = poll.wait(10, ret);
        if (i > 0)
        {
            cc::Block m(1024);
            cc::Address as;
            udp.RecvFrom(m, as, 0);
            std::cout << "recieve: " << std::endl
                      << m.c_str() << std::endl;
            std::cout << "ip: " << as.ipAddress() << " port: " << as.port() << std::endl;
        }
    }
    udp.Close();
}
void ttcp()
{
    auto tcp = cc::Socket::createTCP(cc::ipv4);
    auto localhost = cc::Address(cc::ipv4, 8081);
    tcp.Bind(localhost);
    tcp.Listen(256);
    cc::Poll poll;
    poll.add(tcp.fd(), cc::Poll::IN);
    while (true)
    {
        std::vector<cc::Poll::Result> ret;
        int i = poll.wait(10, ret);
        if (i > 0)
        {
            for (auto &&i : ret)
            {
                if (i.fd == tcp.fd())
                {
                    cc::Address as;
                    int fd = tcp.Accept(as);
                    std::cout << "accept ip: " << as.ipAddress() << " port: " << as.port() << std::endl;
                    poll.add(fd, cc::Poll::IN);
                }
                else
                {
                    cc::Block m(1024);
                    cc::Socket s(i.fd);
                    int c = s.Recieve(m, 0);
                    if (c == 0)
                    {
                        poll.remove(i.fd);
                        s.Close();
                        break;
                    }
                    if (c == -1)
                    {
                        std::cout << strerror(errno) << std::endl;
                        poll.remove(i.fd);
                        s.Close();
                        break;
                    }
                    std::cout << "recieve: " << std::endl
                              << m.c_str() << std::endl;
                }
            }
        }
    }
    tcp.Close();
}
void udp()
{
    cc::Loop p;
    p.dispatch([]()
               { tudp(); });
    std::this_thread::sleep_for(std::chrono::seconds(100));
}
void tcp()
{
    cc::Loop p;
    p.dispatch([]()
               { ttcp(); });
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
}
void pool()
{
    cc::Pool *m = new cc::Pool(10);

    for (size_t i = 0; i < 10000; i++)
    {
        m->dispatch([i]()
                    {
                       std::cout << i << std::endl;
                       std::this_thread::sleep_for(std::chrono::seconds(1)); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete m;
    std::cout << "call delete" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}



void server(){
    cc::TCPServer server(cc::ipv4);
    server.Start(8088);
    auto m = new cc::TCPServerDelegate();
    server.SetDelegate(m);
    server.WaitClose();
}
int main()
{
    server();
}
void loop()
{
    cc::Loop *m = new cc::Loop();

    for (size_t i = 0; i < 10000; i++)
    {
        m->dispatch([i]()
                    {
                       std::cout << i << std::endl;
                       std::this_thread::sleep_for(std::chrono::seconds(1)); });
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete m;
    std::cout << "call delete" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}