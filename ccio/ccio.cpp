#include "ccio.h"
#include "cc/server.hpp"
#include "cc/block.hpp"
using namespace cc;
class ccio_server : public cc::TCPServerDelegate
{
public:
    ccio_server()
    {
    }
    ~ccio_server()
    {
    }

    virtual void onConnect(TCPServer &server, Client &fd, Address &address)
    {
        if(m_ccio->onConnect != nullptr){
            auto addr = address.ipAddress();
            ccio_addr ca = {addr.c_str(), address.port()};
            ccio_client cl = (ccio_client){fd.fd(),ca};
            m_ccio->onConnect(m_ccio->userdata,m_ccio,&cl);
        }
    };
    virtual void onDisconnect(TCPServer &server, Client &fd, const Address &address, const char *msg)
    {
        if(m_ccio->onDisconnect != nullptr){
            auto addr = address.ipAddress();
            ccio_addr ca = {addr.c_str(), address.port()};
            ccio_client cl = (ccio_client){fd.fd(),ca};
            m_ccio->onDisconnect(m_ccio->userdata,m_ccio,&cl);
        }
    };
    virtual void onRead(TCPServer &server, Client &fd, const Address &address, Block &block)
    {
        if(m_ccio->onRead != nullptr){
            auto addr = address.ipAddress();
            ccio_addr ca = {addr.c_str(), address.port()};
            ccio_client cl = (ccio_client){fd.fd(),ca};
            m_ccio->onRead(m_ccio->userdata,m_ccio,&cl,block.data(),block.size());
        }
    };
    virtual void onWrite(TCPServer &server, Client &fd, const Address &address)
    {
        if(m_ccio->onWrite != nullptr){
            auto addr = address.ipAddress();
            ccio_addr ca = {addr.c_str(), address.port()};
            ccio_client cl = (ccio_client){fd.fd(),ca};
            m_ccio->onWrite(m_ccio->userdata,m_ccio,&cl);
        }
    };
    ccio* m_ccio;
};


int ccio_tcp_init(ccio ** cc,uint16_t port,void *userdata)
{
    auto servr = new cc::TCPServer(cc::AddressFamily::ipv4);
    auto delegate = new ccio_server();
    
    servr->SetDelegate(delegate);
    servr->Start(port);
    delegate->m_ccio = *cc;
    (*cc)->context = delegate;
    (*cc)->server = servr;
    (*cc)->userdata = userdata;
    return 0;
}
ccio *create_ccio()
{
    void * ptr = malloc(sizeof(ccio));
    memset(ptr,0,sizeof(ccio));
    return reinterpret_cast<ccio*>(ptr);
}
void ccio_tcp_wait(ccio **cc)
{
   if ((*cc)->server != nullptr){
        reinterpret_cast<TCPServer*>((*cc)->server)->WaitClose();
    } 
}

void ccio_tcp_send(ccio_client *client, const void *data, size_t len)
{
    cc::Socket s(client->fd);
    cc::Block block(data,len);
    s.Send(block,0);
}
void free_ccio(ccio **cc)
{
    if((*cc)->context != nullptr){
        delete reinterpret_cast<ccio_server*>((*cc)->context);
    }
    if ((*cc)->server != nullptr){
        delete reinterpret_cast<TCPServer*>((*cc)->server);
    }
    free(*cc);
    *cc = nullptr;
}

void ccio_tcp_prepare_send(ccio **cc, int fd)
{
    auto server = reinterpret_cast<TCPServer*>((*cc)->server);
    cc::Client c(fd);
    server->PrepareSend(c);
}
