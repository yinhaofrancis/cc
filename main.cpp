#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <signal.h>
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