
#include <iostream>

#include "cc/task.hpp"
#include "cc/select.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "cc/stream.hpp"
#include "main.h"
#include "cc/tcp.hpp"
#include "cc/object.hpp"

class nn : virtual public cc::Object
{
public:
    nn(){}
    virtual ~nn(){
        this->release();
    }
    virtual void dealloc(){
        std::cout<<"nn"<<std::endl;
    }
};
cc::Object mk(cc::Object m){
    std::cout<<m.refCount()<<std::endl;
    return m;
}
void mkk(cc::Object &&m){
    std::cout<<m.refCount()<<std::endl;
}
int main()
{
    
    cc::TcpServer ts(cc::Ipv4); 
    ts.Listen(8080);

    std::this_thread::sleep_for(std::chrono::seconds(300));

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
            if (c > 0)
            {
                std::memset(buf, 0, 64);
                read(fd[0], buf, 64);
                std::cout << buf << std::endl;
            }
            else if (c < 0)
            {
                std::cout << strerror(errno) << std::endl;
            }
            else
            {
                std::cout << "time out" << std::endl;
            }
        }
    }
}