#include <iostream>
#include "rpc/define.hpp"
#include "rpc/stream.hpp"
#include "rpc/socket.hpp"

int main(int, char **)
{
    rpc::file ff("m", rpc::status::creat | rpc::status::rw);
    ff.write("111111", 6);
    std::cout << strerror(errno) << std::endl;
    ff.close();
}
