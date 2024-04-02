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

void m_onConnect(void* userdata,ccio* cc,ccio_client* client){
    std::cout << "connect " << client->addr.ip << " " << client->addr.port << std::endl;
    ccio_tcp_prepare_send(&cc,client->fd);
}
void m_onDisconnect(void* userdata,ccio* cc,ccio_client* client){
    std::cout << "disconnect " << client->addr.ip << " " << client->addr.port << std::endl;
}

void m_onRecieve(void* userdata,ccio* cc,ccio_client* client,const void* data,size_t len){
    char* c = (char*)malloc(len + 1);
    memset(c,0,len + 1);
    memcpy(c,data,len);
    std::cout << "recieve " << client->addr.ip << " " << client->addr.port << std::endl << c << std::endl;
    free(c);
    ccio_udp_send(&cc,client->addr,"hello",5);
    
}void m_onWrite(void* userdata,ccio* cc,ccio_client* client){
    std::cout << "write " << client->addr.ip << " " << client->addr.port << std::endl;
    ccio_tcp_send(client,"hello",5);
}


int main(){
    ccio* c = create_ccio();
    c->onConnect = m_onConnect;
    c->onDisconnect = m_onDisconnect;
    c->onRead = m_onRecieve;
    c->onWrite = m_onWrite;
    ccio_udp_init(&c,8080,nullptr);
    ccio_wait(&c);
    return 0;
}