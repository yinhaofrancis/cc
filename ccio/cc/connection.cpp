#include "connection.hpp"

cc::Connection::Connection(AddressFamily af, SockType sock, Protocol proto):Socket(af,sock,proto)
{

}

int cc::Connection::setTarget(const Address &address)
{
    m_state = ConnectState::Connecting;
    this->m_target_address = address;
    if(m_proto == cc::tcp){
        return Socket::Connect(address);
    }
    return 0;
}

void cc::Connection::SetDelegate(Delegate *delegate)
{
    m_delegate = delegate;
}

void cc::Connection::SetPoll(AsyncPoll &poll) const
{
    poll.add(this->fd(),cc::Poll::Event::IN | cc::Poll::Event::OUT,(AsyncPoll::AsyncPollCallback *)this);
}

void cc::Connection::Send(const Block &block) const
{
    if(m_proto == cc::tcp){
        Socket::Send(block,0);
    }else{
        Socket::SendTo(block,m_target_address,0);
    }
    
}

void cc::Connection::Recieve(Block &block)
{
    if(m_proto == cc::tcp){
        Socket::Recieve(block,0);
    }else{
        Socket::RecvFrom(block,m_target_address,0);
    }
    
}

void cc::Connection::Close()
{
    Socket::Close();
    m_state = Connection::Closed;
}

cc::Address cc::Connection::Target()
{
    return m_target_address;
}

cc::Connection::ConnectState cc::Connection::State()
{
    return m_state;
}

void cc::Connection::onEvent(AsyncPoll &poll, Poll::Result & r)
{
    if(r.revents == Poll::Event::IN){
        Block block(1024);
        Recieve(block);
        if(m_delegate){
            m_delegate->onRead(*this,block);
        }
    }
    if(r.revents == Poll::Event::OUT){
        if(m_delegate){
            m_delegate->onWrite(*this);
        }
    }
}
