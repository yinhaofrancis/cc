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

#include <signal.h>
#include "cc/event.hpp"
#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"

int main(){

    std::vector<cc::EndPoint> out;
    cc::EndPoint::resolute("www.qq.com",443,SOCK_STREAM,out);
    pid_t pid = fork();

    if(pid == 0){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto fd = open("/tmp/cc",O_WRONLY);
        int idex = 0;
        cc::EventQueue timer;
        timer.add_timer(1,cc::EventQueue::SECONDS,1);
        while (true)
        {
            std::string str = std::to_string(idex) + "aaaaa";
            write(fd,str.data(),str.size());
            idex++;
            struct kevent m;
            timer.wait(1,m);
        }
        
    }else{
        int out = mkfifo("/tmp/cc",0666);
        
        if(out < 0){
            std::cout << strerror(errno) << std::endl;
        }
        auto fd = open("/tmp/cc",O_RDONLY);
        if (fd == -1){
            std::cout << strerror(errno) << std::endl;
        }
        auto select = cc::Poll();
        select.add(fd,cc::Poll::ConfigIN);
        char buf[64];
        while (true)
        {
            int c = select.wait(9);
            if(c > 0){
                memset(buf,0,64);
                read(fd,buf,64);
                std::cout << buf << std::endl;
            }else if (c == 0){
                std::cout << "no data" << std::endl;
            }else{
                std::cout << strerror(errno) << std::endl;
            }
        }
    }
}
