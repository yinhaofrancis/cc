
#include <iostream>

#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"
#include "main.h"
#include "cc/tcp.hpp"

class TCPDelegate:virtual public cc::TcpServer::Delegate{
    virtual void recieve(const cc::TcpServer::Client& client,void* buffer, size_t size){
        std::string str((char* )buffer,size);
        std::string info;
        client.ep.info(info);
        std::cout << info << std::endl;
        std::cout << str << std::endl;
    }
    virtual ~TCPDelegate(){
        
    }
};

cc::TcpServer* g_server = nullptr;

void server()
{
    g_server = new cc::TcpServer(cc::AddressFamily::Ipv4);

    g_server->Listen(8080,new TCPDelegate());
}
int main()
{

    server();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(20));
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