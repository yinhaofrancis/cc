#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/select.h>
#include "cc/connection.hpp"
#include "cc/poll.hpp"



// 192.168.84.45:180
int main(){
    auto ap = new cc::AsyncPoll();
    auto c = new cc::Connection(cc::ipv4,cc::stream,cc::tcp);

    cc::Address addr(cc::ipv4,"192.168.84.45",180);
    c->SetPoll(*ap);
    int a = c->setTarget(addr);
    std::cout << a << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(200));
}