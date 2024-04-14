#include <iostream>
#include "rpc/define.hpp"
#include "rpc/stream.hpp"
#include "rpc/socket.hpp"
#include <sys/un.h>
#include <thread>

int main(int, char **)
{

    pid_t p = fork();

    if (p == 0)
    {
        rpc::socket<rpc::domain::unix, rpc::sock::strm, rpc::protocol::none> s;

        rpc::address<rpc::domain::unix> addr("um");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        s.connect(addr);
        const char *buff = "123456";
        s.write(buff, 6);
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
        char buff[1024];

        memset(buff, 0, 1024);

        cliet.read(buff, 1024);

        std::cout << buff << std::endl;

        cliet.close();
        s.close();
    }
}
