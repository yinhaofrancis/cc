#ifndef __CCIO_H__
#define __CCIO_H__

extern "C"{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>


    struct ccio_addr
    {
        const char* ip;
        uint16_t port;
    };
    
    struct  ccio
    {
        void (*onConnect)(void* userdata,int fd,ccio_addr * addr);
        void (*onDisconnect)(void* userdata,int fd,ccio_addr * addr);
        void (*onRead)(void* userdata,int fd,ccio_addr * addr,const void *buffer,size_t size);
        void (*onWrite)(void* userdata,int fd,ccio_addr * addr);
        void * context;
        void * server;
        void * userdata;
    };



    ccio* create_ccio();

    int ccio_init_tcp(ccio ** cc,uint16_t port,void *userData);

    void ccio_wait(ccio **cc);

    void ccio_pending_send(ccio* cc,int fd);

    void free_ccio(ccio**);



}
#endif