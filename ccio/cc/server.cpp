#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"

cc::TCPServer::TCPServer(AddressFamily af) : Socket(af, SockType::stream, Protocol::tcp)
{
    m_pool = new Pool(0);
}

cc::TCPServer::~TCPServer()
{
    this->m_is_running = false;
    delete this->m_pool;
}

void cc::TCPServer::SetDelegate(TCPServerDelegate *delegate)
{
    this->m_pool->dispatch([this, delegate]()
    {
        this->m_is_running = true;
        while (this->m_is_running)
        {
            std::vector<Poll::Result> m_result;
            int ret = this->m_poll.wait(10,m_result);

            if(this->m_is_running == false){
                return;
            }
            if(ret > 0){
                for (auto &result : m_result)
                {
                    if(result.revents & cc::Poll::Event::IN && result.fd == this->fd())
                    {
                        Address inAddress;
                        auto client = this->Accept(inAddress);
                        this->m_map_client[client] = inAddress;
                        if(client == -1)
                        {
                            continue;
                        }
                        this->m_poll.add(client,cc::Poll::Event::IN);
                        Client cclient(client);
                        delegate->onConnect(*this,cclient,inAddress);
                    }else if(result.revents & cc::Poll::Event::IN)
                    {
                        Client client(result.fd);
                        Socket read(result.fd);
                        Block block(1024);
                        Address address = this->m_map_client[result.fd];
                        int c = read.Recieve(block,0);
                        if (c > 0){
                            delegate->onRead(*this,client,address,block);
                        }else if (c == 0){
                            m_map_client.erase(result.fd);
                            delegate->onDisconnect(*this,client,address,nullptr);
                            client.Close();
                        }else if (c == -1){
                            m_map_client.erase(result.fd);
                            delegate->onDisconnect(*this,client,address,strerror(errno));
                            client.Close();
                        }
                    }else if(result.revents & cc::Poll::Event::OUT)
                    {
                        Address& address = this->m_map_client[result.fd];
                        this->m_poll.remove(result.fd,cc::Poll::Event::OUT);
                        Client client(result.fd);
                        delegate->onWrite(*this,client,address);
                    }
                }
            }
        } 
    });
}

void cc::TCPServer::Start(uint16_t port)
{
    auto localhost = cc::Address(this->m_af, port);
    this->Bind(localhost);
    this->Listen(256);
    this->m_poll.add(this->fd(),cc::Poll::Event::IN);    
}

void cc::TCPServer::Stop()
{
    this->m_is_running = false;
}

void cc::TCPServer::WaitClose()
{
    m_pool->Wait();
}

void cc::TCPServer::PrepareSend(Client &client)
{
    m_poll.add(client.fd(), cc::Poll::Event::OUT);
}

cc::Client::Client(int fd) : Socket(fd)
{
    cc::Stream::addStatus(O_NONBLOCK);
}

int cc::Client::fd()
{
    return cc::Stream::fd();
}

ssize_t cc::Client::Send(const Block &block, int flag)
{
    return Socket::Send(block, flag);
}

ssize_t cc::Client::SendTo(const Block &block, const Address &address, int flag)
{
    return Socket::SendTo(block, address, flag);
}

void cc::Client::Close()
{
    Socket::Close();
}
