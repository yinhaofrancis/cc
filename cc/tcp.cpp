#include "tcp.hpp"
#include "task.hpp"
#include "select.hpp"
#include <map>
#include <iostream>

const size_t buffer_size = 4096;

cc::TcpConnectedClient::TcpConnectedClient(int socketfd, sockaddr *address)
:m_stream(socketfd),
m_address(*address),
m_block(new std::vector<Block>())
{

}

cc::TcpConnectedClient::~TcpConnectedClient()
{
    release();
}

void cc::TcpConnectedClient::dealloc()
{ 
    delete m_block;
}

void cc::TcpConnectedClient::Close() const
{
    m_stream.Close();
}

void cc::TcpConnectedClient::Send(const Block &block) const
{
    m_block->push_back(block);
}

void cc::TcpConnectedClient::notifyCanSend()
{
    if (m_block->size() > 0){
        for (auto &&i : *m_block)
        {
            m_stream.Send(i.buffer(),i.size(),0);
        }
        m_block->clear();
    }
}
