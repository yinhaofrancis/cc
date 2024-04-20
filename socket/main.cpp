#include <iostream>
#include "ipc/define.hpp"
#include "ipc/stream.hpp"
#include "ipc/socket.hpp"
#include "ipc/select.hpp"
#include <sys/un.h>
#include <thread>
#include "ipc/event.hpp"
#include <sys/sockio.h>
void pipte();
void dupTest();
void unixf();
int main(int, char **)
{
    rpc::address<rpc::domain::local> addr1("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    addr1.size();

rpc::address<rpc::domain::local> addr2("890");
    addr2.size();


}
void kqueue(){
    #if __APPLE__
     int kq = kqueue();
    signal(SIGINT,SIG_IGN);
    ipc::event e(1,ipc::add,ipc::timer,ipc::millseconds,5000);
    ipc::event s(SIGINT,ipc::add,ipc::signal,ipc::none_note,0);

    e.mount(kq);
    s.mount(kq);
    while (true)
    {
        timespec k;
        k.tv_sec = 10;
        k.tv_nsec = 0;
        std::vector<ipc::event> res;
        int i = ipc::event::wait<2>(kq,&k,res);
        for (auto &&i : res)
        {
            std::cout << i.et << std::endl;
        }
        
    }
    #endif
}
void dupTest()
{
    auto dupout = ipc::stream::dup(STDOUT_FILENO);
    dupout.setStatus(ipc::nonblock);
    auto dupin = ipc::stream::dup(STDIN_FILENO);
    dupin.setStatus(ipc::nonblock);

    timeval k;
    k.tv_sec = 1;
    k.tv_usec = 0;
    ipc::select<ipc::se_in, ipc::stream> s(k, [](ipc::stream &&s)
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
    ipc::pipe p;
    p.rpipe().setStatus(ipc::nonblock);
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
        ipc::select<ipc::se_in, ipc::stream> sl(k, [](ipc::stream &&fd)
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
void unixf()
{
    pid_t p = fork();
    if (p == 0)
    {
        rpc::socket<rpc::domain::local, rpc::sock::strm, rpc::protocol::none> s;

        rpc::address<rpc::domain::local> addr("um");
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
        rpc::socket<rpc::domain::local, rpc::sock::strm, rpc::protocol::none> s;

        rpc::address<rpc::domain::local> addr("um");
        if (s.bind(addr) == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }

        if (s.listen(20) == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }

        rpc::address<rpc::domain::local> client("");

        ipc::stream cliet = s.accept(client);

        if (cliet.fd == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }
        std::cout << "accept " << client.path() << std::endl;

        timeval k;
        k.tv_sec = 1;
        k.tv_usec = 0;
        ipc::select<ipc::se_in, ipc::stream> sl(k, [](ipc::stream fd)
                                                {
                                          
                                          fd.setStatus(ipc::nonblock);
                                          char buff[1024];

                                          memset(buff, 0, 1024);

                                          auto ret = fd.read(buff, 1024);
                                          std::cout << buff << std::endl;
                                          return ret; });
        sl.add(cliet);

        std::this_thread::sleep_for(std::chrono::seconds(300));
    }
}