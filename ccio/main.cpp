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

#include "cc/poll.hpp"
#include "cc/server.hpp"



// 192.168.84.45:180
int main(){
    std::this_thread::sleep_for(std::chrono::seconds(100));
}