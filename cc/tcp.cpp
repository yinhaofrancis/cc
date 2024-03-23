#include "tcp.hpp"
#include "task.hpp"
#include "select.hpp"
#include <map>
#include <iostream>

const size_t buffer_size = 4096;

cc::TcpConnectedClient::TcpConnectedClient(Stream &socketfd, EndPoint &address,TcpServer* server)
    : m_stream(socketfd),
      m_address(address),m_server(server),
      m_block(new std::vector<Block>())
{
    m_stream.setNoBlock();
}

cc::TcpConnectedClient::TcpConnectedClient() : m_stream(0), m_address(0, 0)
{
}

cc::TcpConnectedClient::~TcpConnectedClient()
{
}

void cc::TcpConnectedClient::dealloc()
{
    delete m_block;
}

void cc::TcpConnectedClient::Close() const
{
    
    int fd = m_stream.streamFD();
    if(m_server != nullptr){
        m_server->removeClient(fd);
    }
    m_stream.Close();

}

void cc::TcpConnectedClient::Send(const Block &block) const
{
    m_block->push_back(block);
    m_server->addClientPendingWrite(this->m_stream.streamFD());
}

void cc::TcpConnectedClient::Recieve()
{
    void *m_w = malloc(1024);
    int c = 0;
    do
    {
        c = this->m_stream.Read(m_w, 1024);
        if (c > 0)
        {
            Block bk(m_w, c);
            std::cout << "recieve: " << c << std::endl;
            if(m_delegate != nullptr){
                this->delegate()->recieve(bk);
            }
            this->Send(bk);
        }
    } while (c > 0);
    free(m_w);
}

void cc::TcpConnectedClient::setDelegate(TcpConnectedClientDelegate *delegate)
{
    m_delegate = delegate;
}

cc::TcpConnectedClientDelegate *cc::TcpConnectedClient::delegate()
{
    return m_delegate;
}

void cc::TcpConnectedClient::notifyCanSend()
{
    if (m_block->size() > 0)
    {
        for (auto &&i : *m_block)
        {
            m_stream.Send(i.buffer(), i.size(), 0);
            i.dealloc();
        }
        m_block->clear();
    }
    m_server->m_select->remove(this->m_stream.streamFD(),cc::Select::Config::SelectWrite);
}

cc::TcpServer::TcpServer(AddressFamily af) : m_af(af)
{
    m_server = new Stream();
    Stream::CreateTcp(af, *m_server);
    m_select = new Select();
    m_select->add(m_server->streamFD(), cc::Select::Config::SelectRead);
}

cc::TcpServer::~TcpServer()
{
    this->m_is_running = false;
    delete m_task;
    delete m_server;
    delete m_select;
}

void cc::TcpServer::Listen(uint16_t port)
{
    EndPoint ep(m_af, port);
    m_server->Bind(ep);
    m_server->Listen(64);
    m_is_running = true;
    m_task = new Task([this]()
                      {
        while (this->m_is_running)
        {
            int count = this->m_select->wait(0.1);
            if (count < 0){
                std::cout << strerror(errno) << std::endl;
                this->m_is_running = false; 
            }else if(count > 0){
                std::vector<int> fd;
                this->m_select->occur_read(fd);
                for (auto &&i : fd)
                {
                    if (i == this->m_server->streamFD()){
                        EndPoint ep(this->m_af,0);
                        Stream stream;
                        int ret = this->m_server->Accept(ep,stream);
                        if(ret == -1){
                            std::cout << strerror(errno) << std::endl;
                        }else{
                            std::string addressInfo;
                            ep.info(addressInfo);
                            std::cout << "accept: " << addressInfo << std::endl;
                            TcpConnectedClient tcc(stream,ep,this);
                            this->m_clients[stream.streamFD()] = tcc;
                            this->m_select->add(stream.streamFD(),cc::Select::SelectRead);
                        }
                    }else{
                       this->m_clients[i].Recieve();
                    }  
                }
                fd.clear();
                this->m_select->occur_write(fd);
                for (auto &&i : fd){
                   this->m_clients[i].notifyCanSend(); 
                }

            }
        } });
}

void cc::TcpServer::removeClient(int fd)
{
    m_select->remove(fd);
    m_clients.erase(fd);
}

void cc::TcpServer::addClientPendingWrite(int fd)
{
    m_select->add(fd,cc::Select::Config::SelectWrite);
}
