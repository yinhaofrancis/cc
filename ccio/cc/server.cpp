#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"
#include "poll.hpp"
const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 2048;

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

int cc::Sender::fd() const
{
    return fd();
}

cc::UdpServer::UdpServer(AddressFamily af) : Socket(af, cc::dgram, cc::udp)
{
    Stream::addStatus(O_NONBLOCK);
}

int cc::UdpServer::Listen(uint16_t port)
{
    Address b(m_af, port);
    return Socket::Bind(b);
}

void cc::UdpServer::Close()
{
    Socket::Close();
}

void cc::UdpServer::SetReciever(UdpServerReciever *reciever)
{
    this->m_is_running = true;
    m_loop.dispatch([this, reciever]()
                    {
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
                for (auto &&i : events)
                {
                    if(i.revents & cc::Poll::IN){
                        Address addr;
                        Block buffer(kDefaultReciecveSize);
                        Socket::RecvFrom(buffer,addr,0);
                        reciever->onRecieve(*this,addr,buffer);
                    }else if(i.revents & cc::Poll::OUT) {
                        m_mutex.lock();
                        for (auto &&i : this->m_addresses)
                        {
                            Sender s(m_af,m_st,m_proto,i);
                            m_mutex.unlock();
                            reciever->onSend(*this,s);
                             m_mutex.lock();
                        }
                        this->m_addresses.clear();
                        this->m_poll.remove(fd(),cc::Poll::OUT);
                        m_mutex.unlock();
                    }
                }
            }
        } });
}

void cc::UdpServer::PrepareSender(Address &addre)
{
    m_mutex.lock();
    this->m_poll.add(fd(), cc::Poll::Event::OUT);
    this->m_addresses.push_back(addre);
    m_mutex.unlock();
}

cc::TcpServer::TcpServer(AddressFamily a) : Socket(a, cc::stream, cc::tcp)
{
}

int cc::TcpServer::Listen(uint16_t port)
{
    Address b(m_af, port);
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

void cc::TcpServer::Prepare(int senderfd)
{
    auto it = std::find_if(m_map_client.begin(), m_map_client.end(), [&](auto &i)
                           { return i.second.fd() == senderfd; });
    m_poll.add((*it).second.fd(), cc::Poll::OUT);
}

void cc::TcpServer::SetReciever(TcpServerReciever *reciever)
{
    this->m_is_running = true;
    m_loop.dispatch([this, reciever]()
                    {
        this->m_poll.add(fd(),cc::Poll::IN);
        while (this->m_is_running)
        {
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
                            Sender s(fd,m_af,m_st,m_proto,addr);
                            this->m_map_client[fd] = s;
                            m_poll.add(fd,cc::Poll::IN);
                            reciever->OnConnect(*this,s);
                        }
                    }else {
                        if(i.revents & cc::Poll::IN){
                            Block b(2048);
                            Socket(i.fd).Recieve(b,0);
                            Sender s = this->m_map_client[i.fd];
                            if(b.size() == 0){
                                this->m_map_client.erase(i.fd);
                                reciever->OnDisconnect(*this,s);
                                m_poll.remove(i.fd);
                                s.Close();
                            }else{
                                reciever->onRecieve(*this,s,b);
                            }
                        }
                        if(i.revents & cc::Poll::ERR){
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            reciever->onError(*this,s,strerror(errno));
                            s.Close();
                            m_poll.remove(i.fd);
                        }
                        if(i.revents & cc::Poll::HUP){
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            reciever->onError(*this,s,strerror(errno));
                            s.Close();
                            m_poll.remove(i.fd);
                        }
                        if(i.revents & cc::Poll::NVAL){
                            Sender s = this->m_map_client[i.fd];
                            this->m_map_client.erase(i.fd);
                            reciever->onError(*this,s,strerror(errno));
                            s.Close();
                            m_poll.remove(i.fd);
                        }
                        if(i.revents & cc::Poll::OUT){
                            Sender s = this->m_map_client[i.fd];
                            reciever->onSend(*this,s);
                            m_poll.remove(i.fd,cc::Poll::OUT);
                        }
                    }
                }
            }
        } });
}
