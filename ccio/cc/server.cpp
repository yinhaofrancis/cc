#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"
const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 1024;

cc::Server::Server(AddressFamily af,SockType sock,Protocol proto) : Socket(af,sock, proto)
{
    m_pool = new Pool(0);
}

cc::Server::~Server()
{
    this->m_is_running = false;
    delete this->m_pool;
}

void cc::Server::SetDelegate(ServerDelegate *delegate)
{
    this->m_pool->dispatch([this, delegate]()
                           {
        this->m_is_running = true;
        if(this->m_proto == cc::Protocol::tcp){
            tcp_server_core_process(delegate);
        }else if (this->m_proto == cc::Protocol::udp){
            udp_server_core_process(delegate);
        }
        
    });
}
void cc::Server::udp_server_core_process(cc::ServerDelegate *delegate)
{
    
    while (this->m_is_running)
    {
        std::vector<Poll::Result> m_result;
        int ret = this->m_poll.wait(kDefaultTimeout, m_result);

        if (this->m_is_running == false)
        {
            return;
        }
        if (ret > 0)
        {
            for (auto &result : m_result)
            {
                if (result.revents & cc::Poll::Event::IN && result.fd == this->fd())
                {
                    Address inAddress;
                    Block b(kDefaultReciecveSize);
                    auto size = this->RecvFrom(b,inAddress,0);
                    Client client(this->fd(),inAddress,m_proto);
                    if (size == -1)
                    {
                        continue;
                    }
                    delegate->onRead(*this,client,inAddress,b);
                }
            }
        }
    }
}
void cc::Server::tcp_server_core_process(cc::ServerDelegate *delegate)
{
    while (this->m_is_running)
    {
        std::vector<Poll::Result> m_result;
        int ret = this->m_poll.wait(kDefaultTimeout, m_result);

        if (this->m_is_running == false)
        {
            return;
        }
        if (ret > 0)
        {
            for (auto &result : m_result)
            {
                if (result.revents & cc::Poll::Event::IN && result.fd == this->fd())
                {
                    Address inAddress;
                    auto client = this->Accept(inAddress);
                    this->m_map_client[client] = inAddress;
                    if (client == -1)
                    {
                        continue;
                    }
                    this->m_poll.add(client, cc::Poll::Event::IN);
                    Client cclient(client, inAddress, this->m_proto);
                    delegate->onConnect(*this, cclient, inAddress);
                }
                else if (result.revents & cc::Poll::Event::IN)
                {

                    Socket read(result.fd);
                    Block block(kDefaultReciecveSize);
                    Address address = this->m_map_client[result.fd];
                    Client client(result.fd, address, this->m_proto);
                    int c = read.Recieve(block, 0);
                    if (c > 0)
                    {
                        delegate->onRead(*this, client, address, block);
                    }
                    else if (c == 0)
                    {
                        m_map_client.erase(result.fd);
                        delegate->onDisconnect(*this, client, address, nullptr);
                        client.Close();
                    }
                    else if (c == -1)
                    {
                        m_map_client.erase(result.fd);
                        delegate->onDisconnect(*this, client, address, strerror(errno));
                        client.Close();
                    }
                }
                else if (result.revents & cc::Poll::Event::OUT)
                {
                    Address &address = this->m_map_client[result.fd];
                    this->m_poll.remove(result.fd, cc::Poll::Event::OUT);
                    Client client(result.fd, address, this->m_proto);
                    delegate->onWrite(*this, client, address);
                }
            }
        }
    }
}

int cc::Server::Start(uint16_t port)
{
    int ret = 0;
    auto localhost = cc::Address(this->m_af, port);
    ret = this->Bind(localhost);
    ret = this->Listen(256);
    this->m_poll.add(this->fd(),cc::Poll::Event::IN);   
    return ret; 
}

void cc::Server::Stop()
{
    this->m_is_running = false;
}

void cc::Server::WaitClose()
{
    m_pool->Wait();
}

void cc::Server::PrepareSend(int clientfd)
{
    m_poll.add(clientfd, cc::Poll::Event::OUT);
}

cc::Client cc::Server::PrepareSendTo(Address &address)
{
    return Client(this->fd(), address,m_proto);
}

cc::Client::Client(int fd,const Address &address,Protocol &protocol) : Socket(fd),m_address(address),m_proto(protocol)
{
    cc::Stream::addStatus(O_NONBLOCK);
}

int cc::Client::fd()
{
    return cc::Stream::fd();
}

ssize_t cc::Client::Send(const Block &block, int flag)
{
    if(m_proto == cc::Protocol::tcp){
        return Socket::Send(block, flag);
    }else{
        return Socket::SendTo(block, m_address, flag);
    }
}

void cc::Client::Close()
{
    Socket::Close();
}
