
#include <iostream>

#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"


int server(){
    cc::Select select;
    cc::Stream server(0);
    server.setNoBlock();
    int ret = cc::Stream::CreateTcp(AF_INET,server);
    select.add(server.streamFD(),cc::Select::Config::SelectRead);
    cc::EndPoint localhost(AF_INET,8080);
    ret = server.Bind(localhost);
    ret = server.Listen(10);
    std::cout << "listen.......\n" << localhost.info() << std::endl;
    while (true)
    {
        int c = select.wait(100);
        std::cout << c << std::endl;

        if (c > 0){
            std::cout << c << std::endl;
            cc::EndPoint ep(AF_INET,0);
            cc::Stream sr(0);
            for (size_t i = 0; i < c; i++)
            {
                auto a = server.Accept(ep,sr);
                std::cout << "accept:" << std::endl;
                if(a <= 0){
                    std::cout << strerror(errno) << std::endl;
                }else{
                    std::cout << ep.info() << std::endl;
                }
            }
        }else if (c < 0){
            std::cout << strerror(errno) << std::endl;
        }else{
            std::cout << "no client" << std::endl;
        }
    }
    
}

int main()
{
    
    cc::Task::async([](){
        server();
    });
    
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    return 0;
}