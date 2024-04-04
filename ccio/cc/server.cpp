#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"
#include "poll.hpp"

#include <stdio.h>
#include <string.h>
#include <errno.h>

const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 2048;

cc::Sender::Sender(
    int fd,
    Domain domain,
    SockType st,
    Protocol proto,
    Address &address) : Socket(fd), m_address(address)
{
}

cc::Sender::Sender(
    Domain domain,
    SockType st,
    Protocol proto,
    Address &address) : Socket(domain, st, proto), m_address(address)
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
    if (this->protocol() == cc::tcp)
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

int cc::Sender::fd() const
{
    return Socket::fd();
}

cc::UdpServer::UdpServer(Domain domain) : Socket(domain, cc::dgram, cc::udp)
{
    Stream::addStatus(O_NONBLOCK);
    this->setReusePort(true);
    this->setReuseAddr(true);
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
                    if(i.revents & cc::Poll::IN){
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

cc::TcpServer::TcpServer(Domain a) : Socket(a, cc::stream, cc::tcp)
{
    this->setReusePort(true);
    this->setReuseAddr(true);
}

int cc::TcpServer::Listen(uint16_t port)
{
    Address b(domain(), port);
    int ret = Socket::Bind(b);
    if (ret < 0)
    {
        return ret;
    }
    return Socket::Listen(10);
}

void cc::TcpServer::Close()
{
    Socket::Close();
}

void cc::TcpServer::Prepare(int senderfd, const Block &block)
{
    m_mutex.lock();
    auto cit = std::find_if(m_cache.begin(), m_cache.end(), [senderfd](auto &i)
                            { return i.first == senderfd; });
    if (cit == m_cache.end())
    {
        m_cache[senderfd] = std::vector<Block>();
    }
    auto n = block.copy();
    m_cache[senderfd].push_back(n);

    auto it = std::find_if(m_map_client.begin(), m_map_client.end(), [&](auto &i)
                           { return i.second.fd() == senderfd; });

    m_mutex.unlock();
}

void cc::TcpServer::SetReciever(TcpServer::Reciever *reciever)
{
    this->m_is_running = true;
    m_loop.dispatch([this, reciever]()
                    {
        m_mutex.lock();
        this->m_poll.add(fd(),cc::Poll::IN);
        m_mutex.unlock();
        bool needIdle = true;
        while (this->m_is_running)
        {
            needIdle = true;
            std::vector<Poll::Result> events;
            int ret = this->m_poll.wait(0.2,events);
            if (!this->m_is_running)
            {
                return;
            }
            if(ret > 0){
                for (auto &&i : events)
                {
                    if(i.fd == fd()){
                        if (i.revents & cc::Poll::IN){
                            Address addr;
                            int fd = Socket::Accept(addr);
                            if(fd < 0){
                                continue;
                            }
                            Sender s(fd,domain(),sockType(),protocol(),addr);
                            Stream(fd).addStatus(O_NONBLOCK);
                            m_mutex.lock();
                            this->m_map_client[fd] = s;
                            m_poll.add(fd,cc::Poll::IN);
                            m_poll.add(fd,cc::Poll::OUT);
                            m_mutex.unlock();
                            reciever->onConnect(*this,s);
                            needIdle = false;
                        }
                    }else {
                        if(i.revents & cc::Poll::IN){
                            Block b(2048);
                            Socket(i.fd).Recieve(b,0);
                            m_mutex.lock();
                            Sender s = this->m_map_client[i.fd];
                            if(b.size() == 0){
                                this->m_map_client.erase(i.fd);
                                this->chectRemoveCache(i.fd);
                                m_mutex.unlock();
                                reciever->onDisconnect(*this,s);
                                m_mutex.lock();
                                m_poll.remove(i.fd);
                                s.Close();
                            }else{
                                m_mutex.unlock();
                                reciever->onRecieve(*this,s,b);
                                 m_mutex.lock();
                            }
                            m_mutex.unlock();
                            needIdle = false;
                            b.Free();
                        }
                        if(i.revents & cc::Poll::ERR){
                            m_mutex.lock();
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            this->chectRemoveCache(i.fd);
                            s.Close();
                            m_poll.remove(i.fd);
                            m_mutex.unlock();
                            reciever->onError(*this,s,strerror(errno));
                            needIdle = false;
                        }
                        if(i.revents & cc::Poll::HUP){
                            m_mutex.lock();
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            this->chectRemoveCache(i.fd);
                            s.Close();
                            m_poll.remove(i.fd);
                            m_mutex.unlock();
                            reciever->onError(*this,s,strerror(errno));
                            needIdle = false;
                        }
                        if(i.revents & cc::Poll::NVAL){
                            m_mutex.lock();
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            this->chectRemoveCache(i.fd);
                            s.Close();
                            m_poll.remove(i.fd);
                            m_mutex.unlock();
                            reciever->onError(*this,s,strerror(errno));
                            needIdle = false;
                        }
                        if(i.revents & cc::Poll::OUT){
                            m_mutex.lock();
                            Sender s = this->m_map_client[i.fd];
                            auto m = this->m_cache[i.fd];
                            this->m_cache.erase(i.fd);
                            m_mutex.unlock();
                            for (auto &&i : m)
                            {
                                needIdle = false;
                                s.Send(i);   
                                i.Free();
                            }
                            
                        }
                    }
                }
            }
            if(needIdle){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } });
}

void cc::TcpServer::chectRemoveCache(int senderfd)
{
    auto it = this->m_cache.find(senderfd);
    if (it != this->m_cache.end())
    {
        this->m_cache.erase(it);
    }
}

cc::UdpServer::Message::Message():addre(),block(1)
{
}
