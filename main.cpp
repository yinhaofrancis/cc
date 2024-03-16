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
#include "event.hpp"
#include "task.hpp"
#include "select.hpp"

void child(int fd[2]){
    auto qq = cc::EventQueue();
    qq.add_timer(1,cc::EventQueue::TimerConfig::SECONDS,1);
    int ct = 0;
    while (true)
    {
        if (ct >  3){
            close(fd[0]);
            close(fd[1]);
            exit(0);
        }
        std::vector<struct kevent> e;
        auto state = qq.wait(nullptr,e);
        char c[10] = "";
        snprintf(c,10,"%d",state);
        std::string s = "ddd:";
        s += c;
        s += "\n";
        write(fd[1],s.data(),s.size());
        ct ++;
    }
    
}
void parent(int fd[2]){
    auto qq = cc::EventQueue();
    qq.add_fd_readable(fd[0],0,0);
    char buf[1024];
    while (true)
    {
        struct kevent e;
        int i = qq.wait(0.100,e);
        if (i > 0){
            memset(buf,0,1024);
            read(fd[0],buf,1024);
            std::cout << buf << std::endl;
        }else if(i < 0){
            std::cout << "error  " << strerror(errno) << std::endl;
            exit(1);
        }else{
            std::cout << "time out  " << std::endl;
            exit(0);
        }
    }
    
}
void pipee(){
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if(pid == 0){
        child(pipefd);
    }else{
        parent(pipefd);
    }
}
void testFork(){

    auto pid = fork();
    if(pid == 0){
        int i = 0;
        while (i < 5)
        {
            i++;
            auto pid = fork();
            if (pid == 0){
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return;
            }else{
                std::this_thread::sleep_for(std::chrono::seconds(5));
                i++;
            }
        }
    }else{
        std::cout << "pid: " << pid << std::endl;
        auto qq = cc::EventQueue();
        qq.add_process(pid,cc::EventQueue::EXIT);
        while (true)
        {
            struct kevent e;
            memset(&e,0,sizeof(e));
            int w = qq.wait(10,e);
            if (e.fflags & NOTE_FORK){
                std::cout << e.ident << "fork ok" <<std::endl;
                
                // return;
            }else if (e.fflags & NOTE_EXIT){
                std::cout << e.ident << "exit ok" <<std::endl;
                exit(0);
                // return;
            }else if (e.fflags & NOTE_SIGNAL){
                std::cout << e.ident << "signal ok" <<std::endl;
                // return;
            }else if (e.fflags & NOTE_EXEC){
                std::cout << e.ident << "exec ok" <<std::endl;
                // return;
            }else if (w == 0){
                std::cout << "time out" <<std::endl;
            }else{
                std::cout << "ok  " << std::hex << e.fflags <<std::endl;
            }
        }
        
    }   
    
}
void fk(){
    auto pid = fork();
    if(pid == 0){
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }else{
        auto qq = cc::EventQueue();
        qq.add_process(pid,cc::EventQueue::EXIT);
        while (true)
        {
            struct kevent e;
            int w = qq.wait(1,e);
            if (e.fflags & cc::EventQueue::EXIT){
                std::cout << "ok" <<std::endl;
                return;
            }
            if (w == 0){
                std::cout << "time out" <<std::endl;
            }
        }
    }
}
void testvnode(const char* path){
    auto qq = cc::EventQueue();
    auto fd = open(path,O_RDWR);
    qq.add_vnode(fd,cc::EventQueue::WRITE);
    while (true)
    {
        struct kevent e;
        int r = qq.wait(2,e);
        if(r > 0){
            if(e.fflags == NOTE_WRITE){
                std::cout << "write" << std::endl;
            }
        }else{
            std::cout << "time out" <<std::endl;
        }
    }
    
}
int main(int argc,char* argv[]){
    int fd[2];
    pipe(fd);
    pid_t p = fork();
    cc::Poll m_poll;
    if(p == 0){
        int i = 0;
        while (i < 100)
        {
            std::string s = std::to_string(i);
            write(fd[1],s.data() ,s.size());
            i++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        close(fd[1]);
    }else{
        m_poll.add(fd[0],cc::Poll::Config::ConfigIN);
        while (true)
        {
            int c = m_poll.wait(5);
            if(c == 0){
                std::cout << "timeout" << std::endl;
            }else{
                std::vector<cc::Poll::PollResult> readfds;
                m_poll.occur(readfds);
       
                char buff[64];
                for (auto i = readfds.begin(); i < readfds.end(); i++){
                    memset(buff,0,sizeof(buff));
                    read(i->fd,buff,64);
                    std::cout << buff << std::endl;
                }
            }


        }
        
    }
    return 0;
}


void timer(){
    auto qq = cc::EventQueue();
    qq.add_timer(1,cc::EventQueue::TimerConfig::SECONDS,1);
    qq.add_timer(2,cc::EventQueue::TimerConfig::SECONDS,2);
    qq.add_timer(3,cc::EventQueue::TimerConfig::SECONDS,3);

    while (true)
    {
        struct kevent e;

        auto c = qq.wait(nullptr,e);

        std::cout << "aa " << c << std::endl;
    }
}
