#include "server.hpp"
#include "stream.hpp"
#include "block.hpp"
const TimeInterval kDefaultTimeout = 0.1;
const TimeInterval kDefaultReciecveSize = 1024;

cc::ServerTest::ServerTest(AddressFamily af, SockType sock, Protocol proto) : Socket(af, sock, proto)
{
    m_pool = new Pool(0);
}

cc::ServerTest::~ServerTest()
{
    Socket::Close();
    this->m_is_running = false;
    delete this->m_pool;
}

void cc::ServerTest::SetDelegate(ServerDelegate *delegate)
{
    this->m_pool->dispatch([this, delegate]()
                           {
                               this->m_is_running = true;
                               if (this->m_proto == cc::Protocol::tcp)
                               {
                                   tcp_server_core_process(delegate);
                               }
                               else if (this->m_proto == cc::Protocol::udp)
                               {
                                   udp_server_core_process(delegate);
                               } });
}
void cc::ServerTest::udp_server_core_process(cc::ServerDelegate *delegate)
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
                    auto size = this->RecvFrom(b, inAddress, 0);
                    Sender client(this->fd(), *this, inAddress, m_proto);
                    if (size == -1)
                    {
                        continue;
                    }
                    delegate->onRead(*this, client, inAddress, b);
                }
            }
        }
    }
}
void cc::ServerTest::tcp_server_core_process(cc::ServerDelegate *delegate)
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
                    Sender cclient(client, *this, inAddress, this->m_proto);
                    delegate->onConnect(*this, cclient, inAddress);
                }
                else if (result.revents & cc::Poll::Event::IN)
                {

                    Socket read(result.fd);
                    Block block(kDefaultReciecveSize);
                    Address address = this->m_map_client[result.fd];
                    Sender client(result.fd, *this, address, this->m_proto);
                    int c = read.Recieve(block, 0);
                    if (c > 0)
                    {
                        delegate->onRead(*this, client, address, block);
                    }
                    else if (c == 0)
                    {
                        delegate->onDisconnect(*this, client, address, nullptr);
                        this->RemoveSender(client);
                    }
                    else if (c == -1)
                    {

                        delegate->onDisconnect(*this, client, address, strerror(errno));
                        this->RemoveSender(client);
                    }
                }
                else if (result.revents & cc::Poll::Event::OUT)
                {
                    Address &address = this->m_map_client[result.fd];
                    this->m_poll.remove(result.fd, cc::Poll::Event::OUT);
                    Sender client(result.fd, *this, address, this->m_proto);
                    delegate->onWrite(*this, client, address);
                }
            }
        }
    }
}

int cc::ServerTest::Start(uint16_t port)
{
    int ret = 0;
    auto localhost = cc::Address(this->m_af, port);
    ret = this->Bind(localhost);
    ret = this->Listen(256);
    this->m_poll.add(this->fd(), cc::Poll::Event::IN);
    return ret;
}

void cc::ServerTest::Stop()
{
    this->m_is_running = false;
}

void cc::ServerTest::WaitClose()
{
    m_pool->Wait();
}

void cc::ServerTest::PrepareSend(int clientfd)
{
    m_poll.add(clientfd, cc::Poll::Event::OUT);
}

cc::ServerTest::Sender cc::ServerTest::ServerSender(Address &address)
{
    return Sender(this->fd(), *this, address, m_proto);
}

void cc::ServerTest::RemoveSender(Sender &sender)
{
    if (m_proto == cc::tcp)
    {
        if (sender.fd() != this->fd())
        {
            m_map_client.erase(sender.fd());
            m_poll.remove(sender.fd());
        }
    }
}
cc::ServerTest::Sender::Sender(int fd, ServerTest &server, const Address &address, Protocol &protocol) : Socket(fd), m_server(server), m_address(address), m_proto(protocol)
{
    cc::Stream::addStatus(O_NONBLOCK);
}

int cc::ServerTest::Sender::fd()
{
    return cc::Stream::fd();
}

ssize_t cc::ServerTest::Sender::Send(const Block &block, int flag)
{
    if (m_proto == cc::Protocol::tcp)
    {
        return Socket::Send(block, flag);
    }
    else
    {
        return Socket::SendTo(block, m_address, flag);
    }
}

void cc::ServerTest::Sender::Close()
{
    m_server.RemoveSender(*this);
    Socket::Close();
}
