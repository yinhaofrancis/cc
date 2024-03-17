#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <signal.h>
#include "cc/event.hpp"
#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"

int main()
{

    std::vector<cc::EndPoint> out;
    cc::Poll poll;
    auto m = cc::EndPoint::resolute("baidu.com", SOCK_STREAM, out);
    cc::EndPoint cep(AF_INET,"192.168.1.1",8080);

    std::cout << cep.info() << std::endl;

    for (auto i = out.begin() ; i < out.end(); i++)
    {
        std::cout << i->info() << std::endl;
    }
    int fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    fcntl(fd,O_NONBLOCK);
    poll.add(fd,cc::Poll::ConfigOUT);
    out[0].set_port(80);
    cc::Stream s(fd);
    int ret = s.Connect(out[0]);
    while (true)
    {
        auto c = poll.wait(10);
        if (c > 0){
            std::vector<cc::Poll::PollResult> result;
            poll.occur(result);
            for (auto i = result.begin(); i < result.end(); i++){
                cc::Stream stream = (i->fd);
                std::string n = "abcd";
                stream.Send(n.data(),n.size(),0);
            }
        }
    }
    
    std::cout << ret << std::endl;
    #ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
        std::cout << "This is macOS." << std::endl;
    #else
        std::cout << "This is not macOS." << std::endl;
    #endif
}

int pp()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto fd = open("/tmp/cc", O_WRONLY);
        int idex = 0;
        cc::EventQueue timer;
        timer.add_timer(1, cc::EventQueue::SECONDS, 1);
        while (true)
        {
            std::string str = std::to_string(idex) + "aaaaa";
            write(fd, str.data(), str.size());
            idex++;
            struct kevent m;
            timer.wait(1, m);
        }
    }
    else
    {
        int out = mkfifo("/tmp/cc", 0666);

        if (out < 0)
        {
            std::cout << strerror(errno) << std::endl;
        }
        auto fd = open("/tmp/cc", O_RDONLY);
        if (fd == -1)
        {
            std::cout << strerror(errno) << std::endl;
        }
        auto select = cc::Poll();
        select.add(fd, cc::Poll::ConfigIN);
        char buf[64];
        while (true)
        {
            int c = select.wait(9);
            if (c > 0)
            {
                memset(buf, 0, 64);
                read(fd, buf, 64);
                std::cout << buf << std::endl;
            }
            else if (c == 0)
            {
                std::cout << "no data" << std::endl;
            }
            else
            {
                std::cout << strerror(errno) << std::endl;
            }
        }
    }
}
