#include "tcp.hpp"
#include "task.hpp"
#include "select.hpp"

#include <iostream>

cc::TcpServer::TcpServer(AddressFamily af, TcpServerDelegate *delegate) : m_delegate(delegate), m_af(af)
{
    m_stream = new Stream(0);
    Stream::CreateTcp(af, *m_stream);
}

cc::TcpServer::~TcpServer()
{
    m_stream->Close();
}

void cc::TcpServer::Listen(uint16_t port)
{
    EndPoint ep(m_af, port);
    m_stream->Bind(ep);
    m_stream->Listen(64);
    this->m_is_running = true;
    Select *select = new Select();
    select->add(m_stream->streamFD(), cc::Select::SelectRead);
    Task::async([this, select]()
                {
        while (this->m_is_running)
        {
            int count = select->wait(10);
            if(count > 0){
                EndPoint ep(this->m_af,0);
                Stream clientStream(0);
                this->m_stream->Accept(ep,clientStream);
                if(this->m_delegate != nullptr){
                    this->m_delegate->acceptClient(*this,clientStream,ep);
                }
            }else if (count == 0){
                std::cout << "no client come in " << std::endl;
            }else if (count == -1){
                std::cerr << "occur error" << strerror(errno) << std::endl;
            }
        }
        this->m_stream->Close();
        delete select; });
}

cc::TcpConnectedClient::TcpConnectedClient(Stream &stream, EndPoint &point) : m_endpoint(point)
{
    m_stream = new Stream(stream);
    m_pendding_buffer = new std::vector<PendingBuffer>();
}

cc::TcpConnectedClient::~TcpConnectedClient()
{
    m_stream->Close();
    delete m_stream;
    delete m_pendding_buffer;
}

void cc::TcpConnectedClient::SendBuffer(const void *buffer, const size_t size) const
{
    PendingBuffer pb;
    pb.buffer = buffer;
    pb.size = size;
    this->m_pendding_buffer->push_back(pb);
}

void cc::TcpConnectedClient::SendString(const std::string buffer) const
{
    cc::TcpConnectedClient::SendBuffer(buffer.data(), buffer.size());
}

void cc::TcpConnectedClient::SetDelegate(TcpConnectedClientDelegate *delegate)
{
    m_delegate = delegate;
}

void cc::TcpConnectedClient::Close()
{
    m_is_running = false;
}
