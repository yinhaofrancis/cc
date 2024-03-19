
#include <iostream>

#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"
#include "main.h"
#include "cc/tcp.hpp"

class tcpServerImp:virtual public cc::TcpServerDelegate{
    virtual void acceptClient(const cc::TcpServer& server,cc::Stream& stream,cc::EndPoint& point){
        
        std::string str;
        point.info(str);
        std::cout << str <<std::endl;
    }
};

cc::TcpServer *g_server;

cc::TcpServerDelegate* delegate;


void server()
{
    delegate = new tcpServerImp();
    g_server = new cc::TcpServer(cc::Ipv4,delegate);
    g_server->Listen(8080);
    // g_server.listen(2,8080);
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