#include <iostream>
#include "ipc/define.hpp"
#include "ipc/stream.hpp"
#include "ipc/socket.hpp"

int main(int, char**){
    ipc::address<ipc::ipv4> ip("192.168.2.1",8080);
    ipc::address<ipc::ipv6> ip6("fe80::80b:8aeb:7b1a:306e",8080);
    std::cout << ip.ip_address() << std::endl << ip.port() << std::endl << ip.size() << std::endl;
    std::cout << ip6.ip_address() << std::endl << ip6.port() << std::endl << ip6.size()<< std::endl;
    ipc::socket<ipc::ipv4,ipc::stream,ipc::tcp> tcp;
    tcp.bind(ip);
    tcp.listen(10);
    tcp.write("hello world",1000);
}

