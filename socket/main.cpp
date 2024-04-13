#include <iostream>
#include "ipc/define.hpp"
#include "ipc/stream.hpp"
#include "ipc/socket.hpp"

int main(int, char **)
{
    ipc::file ff("m", ipc::status::creat | ipc::status::rw);
    ff.write("111111", 6);
    std::cout << strerror(errno) << std::endl;
    ff.close();
}
