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
#include "main.h"
#include "ccio.h"

void m_onConnect(void* userdata,int fd,ccio_addr* addr){
    std::cout << "connect " << addr->ip << " " << addr->port << std::endl;
}
void m_onDisconnect(void* userdata,int fd,ccio_addr* addr){
    std::cout << "disconnect " << addr->ip << " " << addr->port << std::endl;
}

int main(){
    ccio* c = create_ccio();
    c->onConnect = m_onConnect;
    c->onDisconnect = m_onDisconnect;
    ccio_init_tcp(&c,8080,nullptr);
    ccio_wait(&c);
    return 0;
}