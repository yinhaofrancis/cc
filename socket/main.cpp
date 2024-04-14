#include <iostream>
#include "rpc/define.hpp"
#include "rpc/stream.hpp"
#include "rpc/socket.hpp"
#include <sys/un.h>
#include <Kernel/sys/un.h>

int main(int, char **)
{

    rpc::file::mkfifo("n",0777);
    rpc::file ff("n", rpc::status::creat | rpc::status::rw);
    std::cout << sizeof(sockaddr_un) << std::endl << sizeof(sockaddr_in6);
    ff.write("111111", 6);
    std::cout << strerror(errno) << std::endl;
    ff.close();
}
