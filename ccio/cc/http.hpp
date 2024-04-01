#include "server.hpp"

namespace cc
{
    class http_server_delegate:public TCPServerDelegate
    {
    public:
        virtual void onConnect(TCPServer& server, Client &fd,Address& address);
        virtual void onDisconnect(TCPServer& server,Client &fd,const Address &address,const char* msg);
        virtual void onRead(TCPServer& server,Client &fd,const Address &address,Block& block);
        virtual void onWrite(TCPServer& server,Client &fd,const Address &address);

    };
} // namespace cc
