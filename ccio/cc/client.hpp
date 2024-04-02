#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "socket.hpp"
namespace cc
{

    class ClientDelegate
    {
    public:
        virtual void onConnect(Client &client, Address &address){};
        virtual void onDisconnect(Client &client, const Address &address, const char *msg){};
        virtual void onRead(Client &client, const Address &address, Block &block){};
        virtual void onWrite(Client &client, const Address &address){};
    };
    class Client
    {
    public:
        
    };
} // namespace cc

#endif