#include "udp.hpp"
#include "stream.hpp"
#include "block.hpp"
#include "poll.hpp"

#include <stdio.h>
#include <string.h>
#include <errno.h>

const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 2048;



cc::UdpServer::UdpServer(Domain domain) : Socket(domain, cc::dgram, cc::udp)
{
    Stream::addStatus(O_NONBLOCK);
    // this->setReusePort(true);
    // this->setReuseAddr(true);
}

int cc::UdpServer::Listen(uint16_t port)
{
    Address b(domain(), port);
    return Socket::Bind(b);
}

void cc::UdpServer::Close()
{
    Socket::Close();
}

void cc::UdpServer::SetReciever(UdpServer::Reciever *reciever)
{
    this->m_is_running = true;
    m_loop.dispatch([this, reciever]()
                    {
        // this->m_poll.add(fd(),cc::Poll::PRI);
        this->m_poll.add(fd(),cc::Poll::IN);
        this->m_poll.add(fd(),cc::Poll::OUT);
        while (this->m_is_running)
        {
            std::vector<Poll::Result> events;
            int ret = this->m_poll.wait(0.1,events);
            if (!this->m_is_running)
            {
                return;
            }
            if(ret > 0){
                for (auto &&i : events)
                {
                    if(i.revents & cc::Poll::IN || i.revents & cc::Poll::PRI){
                        Address addr;
                        Block buffer(kDefaultReciecveSize);
                        Socket::RecvFrom(buffer,addr,0);
                        reciever->onRecieve(*this,addr,buffer);
                        buffer.Free();
                    }else if(i.revents & cc::Poll::OUT) {
                        m_mutex.lock();
                        if(this->m_message.size() == 0){
                            m_mutex.unlock();
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));

                            continue;
                        }
                        for (auto &&i : this->m_message)
                        {
                            m_mutex.unlock();
                            Socket::SendTo(i.block,i.addre,0);
                             m_mutex.lock();
                        }
                        this->m_message.clear();
                        m_mutex.unlock();
                    }
                }
            }
        } });
}

void cc::UdpServer::Send(Address &addre,const Block& block)
{
    m_mutex.lock();
    Message msg;
    msg.addre = addre;
    msg.block.assign(block.data(),block.size());
    this->m_message.push_back(msg);
    m_mutex.unlock();
}

void cc::UdpServer::RecvFrom(Address &addre, Block &b)
{
    Socket::removeStatus(O_NONBLOCK);
    Socket::RecvFrom(b,addre,0);
}

cc::UdpServer::Message::Message():addre(),block(1)
{
}
