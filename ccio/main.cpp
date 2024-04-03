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

class Skeleton : public cc::UdpServerReciever,public cc::TcpServerReciever

{

public:
    virtual void onRecieve(cc::UdpServer &server, cc::Address &addre, const cc::Block &block)
    {
        server.PrepareSender(addre);
    }
    virtual void onSend(cc::UdpServer &server, cc::Sender &sender)
    {
        cc::Block b("hello");
        sender.Send(b);
    }
    virtual void onConnect(cc::TcpServer& server,cc::Sender& sender) {
        std::cout << sender.address().ipAddress() << " connect"<< std::endl;
        sender.Send(cc::Block("hello"));
    }
    virtual void onDisconnect(cc::TcpServer& server,cc::Sender& sender) {
        std::cout << sender.address().ipAddress() << " disconnect"<< std::endl;
    }
    virtual void onRecieve(cc::TcpServer &server,cc::Sender& sender,const cc::Block& block) {
        std::cout << sender.address().ipAddress()<< std::endl << block.c_str() << std::endl;
        server.Prepare(sender.fd());
    }
    virtual void onSend(cc::TcpServer &server,cc::Sender &sender) {
        std::cout << sender.address().ipAddress() << " send someting" << std::endl;
        sender.Send(cc::Block("I Recieved something"));
    }
    virtual void onError(cc::TcpServer &server,cc::Sender& sender,const char* errmsg) {
        std::cout << sender.address().ipAddress() << " " << errmsg << std::endl;
    }
};
// 192.168.84.45:180
int main()
{

    cc::TcpServer us(cc::AddressFamily::ipv4);
    us.SetReciever(new Skeleton());
    int ret = us.Listen(8080);
    if(ret){
        std::cout << strerror(errno) << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(100));
}