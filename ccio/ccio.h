#ifndef __CCIO_H__
#define __CCIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct ccio_addr
{
    const char *ip;
    uint16_t port;
};
struct ccio_client
{
    int fd;
    ccio_addr addr;
};

struct ccio
{
    void (*onConnect)(void *userdata, ccio *cc, ccio_client *client);
    void (*onDisconnect)(void *userdata, ccio *cc, ccio_client *client);
    void (*onRead)(void *userdata, ccio *cc, ccio_client *client, const void *buffer, size_t size);
    void (*onWrite)(void *userdata, ccio *cc, ccio_client *client);
    void *context;
    void *server;
    void *userdata;
};

ccio *create_ccio();

void ccio_wait(ccio **cc);

void free_ccio(ccio **);

int ccio_tcp_init(ccio **cc, uint16_t port, void *userData);

void ccio_tcp_prepare_send(ccio **cc, int fd);

void ccio_tcp_send(ccio_client *client, const void *data, size_t len);


int ccio_udp_init(ccio **cc, uint16_t port, void *userData);

void ccio_udp_send(ccio **cc, ccio_addr address,const void* block,size_t size);

#endif