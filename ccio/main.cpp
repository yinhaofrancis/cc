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
#include "cc/event.hpp"


void tudp()
{
    auto udp = cc::Socket::createUDP(cc::ipv4);
    auto localhost = cc::Address(cc::ipv4, 8080);
    udp.Bind(localhost);
    udp.Listen(256);
    cc::Poll poll;
    poll.add(udp.fd(), POLLIN);
    while (true)
    {
        int i = poll.wait(10);
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

void udp()
{
    cc::Loop p;
    p.dispatch([]()
               { tudp(); });
    std::this_thread::sleep_for(std::chrono::seconds(100));
    
}

int main()
{
    cc::Pool *p = new cc::Pool(0);
    for (size_t i = 0; i < 1000; i++)
    {
        p->dispatch([i](){
            std::cout << i << std::endl;
        });
        
    }
    cc::EventQueue eq;
    eq.add_timer(1,cc::EventQueue::SECONDS,1);
    while (true)
    {
        struct kevent event;
        int i = eq.wait(0.5,event);
        if(i > 0){
            std::cout<< event.ident << std::endl;
        }
    }
    

    std::this_thread::sleep_for(std::chrono::seconds(1));
    delete p;
    
    std::this_thread::sleep_for(std::chrono::seconds(100));
    

}