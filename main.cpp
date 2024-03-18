
#include <iostream>

#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"
#include "main.h"

int server()
{
    cc::Select select;
    cc::Stream server(0);
    int ret = cc::Stream::CreateTcp(AF_INET, server);
    // server.setNoBlock();
    select.add(server.streamFD(), cc::Select::Config::SelectRead);

    cc::EndPoint localhost(AF_INET, 8080);
    ret = server.Bind(localhost);
    ret = server.Listen(10);

    std::cout << "listen.......\n"
              << localhost.info() << std::endl;
    while (true)
    {

        int c = select.wait(6);
        std::cout << c << std::endl;
        if (c > 0)
        {
            cc::EndPoint ep(AF_INET, 0);
            cc::Stream sr(0);
            auto a = server.Accept(ep, sr);
            std::cout << "accept:" << std::endl;
            if (a <= 0)
            {
                std::cout << strerror(errno) << std::endl;
            }
            else
            {
                std::cout << ep.info() << std::endl;
            }
        }
    }
}
int main()
{

    cc::Task::async([]()
                    { server(); });
    // testPipe();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    return 0;
}
void testPipe()
{
    int fd[2];
    pipe(fd);
    pid_t p = fork();
    if (p == 0)
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(20));
            write(fd[1], "aaaa", 4);
            
        }
    }
    else
    {
        cc::Select s;
        s.add(fd[0], cc::Select::Config::SelectRead);
        char buf[64];
        while (true)
        {
            int c = s.wait(1);
            if (c > 0){
                std::memset(buf,0,64);
                read(fd[0], buf, 64);
                std::cout << buf << std::endl;
            }else if (c < 0){
                std::cout << strerror(errno) << std::endl;
            }else{
                std::cout << "time out" << std::endl;
            }
        }
    }
}