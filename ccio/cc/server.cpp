#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"
#include "poll.hpp"
const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 1024;

cc::Sender::Sender(
    int fd,
    AddressFamily af,
    SockType st,
    Protocol proto,
    Address &address) : Socket(fd), m_address(address)
{
    m_af = af;
    m_st = st;
    m_proto = proto;
}

cc::Sender::Sender(
    AddressFamily af,
    SockType st,
    Protocol proto,
    Address &address) : Socket(af, st, proto), m_address(address)
{
}

cc::Sender::Sender() : Socket(0)
{
}

void cc::Sender::Close() const
{
    Socket::Close();
}

int cc::Sender::Send(const Block &block) const
{
    if (m_proto == cc::tcp)
    {
        return Socket::Send(block, 0);
    }
    else
    {
        return Socket::SendTo(block, m_address, 0);
    }
}

cc::Address cc::Sender::address() const
{
    return m_address;
}

cc::UdpServer::UdpServer(AddressFamily af):Socket(af,cc::stream,cc::udp)
{
    Stream::addStatus(O_NONBLOCK);
}

int cc::UdpServer::Listen(uint16_t port)
{
    Address b(m_af,port);
    int ret = Socket::Bind(b);
    if (ret < 0){
        return ret;
    }
    return Socket::Listen(256);
}

void cc::UdpServer::Close()
{
    Socket::Close();
}

void cc::UdpServer::SetReciever(UdpServerReciever *reciever)
{
    this->m_is_running = true;
    m_loop.dispatch([this,reciever](){
        this->m_poll.add(fd(),cc::Poll::IN);
        while (this->m_is_running)
        {
            std::vector<Poll::Result> events;
            int ret = this->m_poll.wait(20,events);
            if (!this->m_is_running)
            {
                return;
            }
            if(ret > 0){

            }
        }
        
    });
}
