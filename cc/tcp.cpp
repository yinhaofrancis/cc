#include "tcp.hpp"
#include "task.hpp"
#include "select.hpp"
#include <map>
#include <iostream>

const size_t buffer_size = 4096;

void cc::TcpServer::Listen(uint16_t port,Delegate *delegate)
{
    Context *c_ctx = new Context();
    c_ctx->af = this->m_af;
    if (m_ctx != nullptr){
        delete m_ctx;
    }
    m_ctx = c_ctx;
    c_ctx->m_runing = true;
    Task::async([delegate,c_ctx,this](){
        void *buffer;
        buffer = malloc(buffer_size);
        while (c_ctx->m_runing){
            int c = c_ctx->clientSelect.wait(1);
            if(c > 0){
                this->RecieveDataProcess(c_ctx, buffer, delegate);
                this->ErrorStreamProcess(c_ctx);
            }
        }
        delete delegate;
    });
    Task::async([c_ctx, port](){
        Stream::CreateTcp(c_ctx->af,c_ctx->stream);
        c_ctx->stream.Bind(EndPoint(c_ctx->af,port));
        c_ctx->stream.Listen(64);
        c_ctx->select.add(c_ctx->stream.streamFD(),cc::Select::Config::SelectRead);
        while (c_ctx->m_runing)
        {
            int count = c_ctx->select.wait(1);
            if(count > 0){
                EndPoint ep(c_ctx->af,0);
                Stream clientSteam;
                int ret = c_ctx->stream.Accept(ep,clientSteam);
                if(ret < 0){
                    std::cout << strerror(errno) << std::endl;
                }else{
                    Client c = (Client){clientSteam,ep,true};
                    c_ctx->lock.lock();
                    c_ctx->clients[clientSteam.streamFD()] = c;
                    c_ctx->clientSelect.add(clientSteam.streamFD(),cc::Select::Config::SelectRead);
                    c_ctx->lock.unlock();
                }
            }else if (count < 0){
                std::cout << strerror(errno) << std::endl;
            }else{
                
            }
        }
        c_ctx->stream.Close();
        delete c_ctx; 
    
    });
}

void cc::TcpServer::ErrorStreamProcess(cc::TcpServer::Context *c_ctx)
{
    std::vector<int> error;
    c_ctx->clientSelect.occur_error(error);
    for (auto i = error.begin(); i < error.end(); i++)
    {
        c_ctx->lock.lock();
        auto client = c_ctx->clients[*i];
        c_ctx->lock.unlock();
        client.isInvalid = false;
        c_ctx->lock.lock();
        c_ctx->clients.erase(client.stream.streamFD());
        c_ctx->lock.unlock();
        client.stream.Close();
    }
}

void cc::TcpServer::RecieveDataProcess(cc::TcpServer::Context *c_ctx, void *buffer, Delegate *delegate)
{
    std::vector<int> reader;
    c_ctx->clientSelect.occur_read(reader);
    for (auto i = reader.begin(); i < reader.end(); i++)
    {
        c_ctx->lock.lock();
        auto client = c_ctx->clients[*i];
        c_ctx->lock.unlock();
        auto result = client.stream.Read(buffer, buffer_size);
        if (result > 0)
        {
            delegate->recieve(client, buffer, result);
        }
        else if (result < 0)
        {
            client.isInvalid = false;
            c_ctx->lock.lock();
            c_ctx->clients.erase(client.stream.streamFD());
            c_ctx->lock.unlock();
            client.stream.Close();
        }
    }
}

bool cc::TcpServer::isRunning()
{
    return this->m_ctx->m_runing;
}

cc::TcpServer::TcpServer(AddressFamily af) : m_af(af)
{
}

cc::TcpServer::~TcpServer()
{
    m_ctx->m_runing = false;
    m_ctx = 0;
}

cc::TcpServer::Delegate::~Delegate()
{
}
