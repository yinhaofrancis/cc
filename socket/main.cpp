#include <iostream>
#include "rpc/define.hpp"
#include "rpc/stream.hpp"
#include "rpc/socket.hpp"
#include "rpc/select.hpp"
#include <sys/un.h>
#include <thread>
void pipte();
void dupTest();
int main(int, char **)
{
    pipte();
}

void dupTest()
{
    auto dupout = rpc::stream::dup(STDOUT_FILENO);
    dupout.setStatus(rpc::nonblock);
    auto dupin = rpc::stream::dup(STDIN_FILENO);
    dupin.setStatus(rpc::nonblock);

    timeval k;
    k.tv_sec = 1;
    k.tv_usec = 0;
    rpc::select<rpc::se_in, rpc::stream> s(k, [](rpc::stream &&s)
                                           {
        char u[128];
        memset(u, 0, 128);
        s.read(u, 128);
        std::cout << u << std::endl;
        return 1; });
    s.add(dupin);

    while (true)
    {

        std::this_thread::sleep_for(std::chrono::seconds(1));
        dupout.write("hello world\n", 12);
    }
}

void pipte()
{
    rpc::pipe p;
    p.rpipe().setStatus(rpc::nonblock);
    pid_t pid = fork();

    if (pid == 0)
    {
        int c = 10;
        while (c > 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            p.wpipe().write("a123456789", 10);
            c--;
        }
    }
    else
    {
        timeval k;
        k.tv_sec = 1;
        k.tv_usec = 0;
        rpc::select<rpc::se_in, rpc::stream> sl(k, [](rpc::stream &&fd)
                                                {
            char k[100];
            memset(k,0,100);
            fd.read(k,100);
            std::cout << k << std::endl; 
            return 1;
        });
            
        sl.add(p.rpipe());
        std::this_thread::sleep_for(std::chrono::seconds(1000));
    }
}
void unix()
{
    pid_t p = fork();
    if (p == 0)
    {
        rpc::socket<rpc::domain::unix, rpc::sock::strm, rpc::protocol::none> s;

        rpc::address<rpc::domain::unix> addr("um");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        s.connect(addr);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int c = 100;

        while (c > 0)
        {
            const char *buff = "123456";
            s.write(buff, 6);
            c--;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        s.close();
    }
    else if (p > 0)
    {
        rpc::socket<rpc::domain::unix, rpc::sock::strm, rpc::protocol::none> s;

        rpc::address<rpc::domain::unix> addr("um");
        if (s.bind(addr) == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }

        if (s.listen(20) == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }

        rpc::address<rpc::domain::unix> client("");

        rpc::stream cliet = s.accept(client);

        if (cliet.fd == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }
        std::cout << "accept " << client.path() << std::endl;

        timeval k;
        k.tv_sec = 1;
        k.tv_usec = 0;
        rpc::select<rpc::se_in, rpc::stream> sl(k, [](rpc::stream fd)
                                                {
                                          
                                          fd.setStatus(rpc::nonblock);
                                          char buff[1024];

                                          memset(buff, 0, 1024);

                                          auto ret = fd.read(buff, 1024);
                                          std::cout << buff << std::endl;
                                          return ret; });
        sl.add(cliet);

        std::this_thread::sleep_for(std::chrono::seconds(300));
    }
}