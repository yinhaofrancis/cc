#include "connection.hpp"

// cc::Connection::Connection(Domain af, SockType sock, Protocol proto):Socket(af,sock,proto)
// {

// }

// int cc::Connection::setTarget(const Address &address)
// {
//     m_state = ConnectState::Connecting;
//     this->m_target_address = address;
//     if(m_proto == cc::tcp){
//         return Socket::Connect(address);
//     }
//     return 0;
// }

// void cc::Connection::SetDelegate(Delegate *delegate)
// {
//     m_delegate = delegate;
// }

// void cc::Connection::SetPoll(AsyncPoll &poll) const
// {
//     poll.add(this->fd(),cc::Poll::Event::IN | cc::Poll::Event::OUT,(AsyncPoll::AsyncPollCallback *)this);
// }

// void cc::Connection::Send(const Block &block) const
// {
//     if(m_proto == cc::tcp){
//         Socket::Send(block,0);
//     }else{
//         Socket::SendTo(block,m_target_address,0);
//     }

// }

// void cc::Connection::Recieve(Block &block)
// {
//     if(m_proto == cc::tcp){
//         Socket::Recieve(block,0);
//     }else{
//         Socket::RecvFrom(block,m_target_address,0);
//     }

// }

// void cc::Connection::Close()
// {
//     Socket::Close();
//     m_state = Connection::Closed;
// }

// cc::Address cc::Connection::Target()
// {
//     return m_target_address;
// }

// cc::Connection::ConnectState cc::Connection::State()
// {
//     return m_state;
// }

// void cc::Connection::onEvent(AsyncPoll &poll, Poll::Result & r)
// {
//     if(r.revents == Poll::Event::IN){
//         Block block(1024);
//         Recieve(block);
//         if(m_delegate){
//             m_delegate->onRead(*this,block);
//         }
//     }
//     if(r.revents == Poll::Event::OUT){
//         if(m_delegate){
//             m_delegate->onWrite(*this);
//         }
//     }
// }

cc::Connection::Connection(Domain domain, SockType sock, Protocol proto, Address &address) : Socket(domain, sock, proto), m_address(address)
{
    if (proto == cc::Protocol::tcp)
    {
        Socket::Connect(address);
    }
    m_ap.add(fd(), cc::Poll::IN, this);
}

void cc::Connection::onEvent(AsyncPoll &poll, cc::Poll::Result &result)
{
    if (result.revents & cc::Poll::IN)
    {
        if (this->m_delegate != nullptr)
        {
            Block b(1024);
            if (protocol() == cc::tcp)
            {
                int ret = Socket::Recieve(b, 0);
                if (ret < 0)
                {
                    return;
                }
                this->m_delegate->onRead(*this, b);
            }
            else if (protocol() == cc::udp)
            {
                Address addr;
                int ret = Socket::RecvFrom(b, addr, 0);
                if (ret < 0)
                {
                    return;
                }
                this->m_delegate->onRead(*this, b, addr);
            }
        }
    }
    if (result.revents & cc::Poll::OUT)
    {
        m_lock.lock();
        auto size = m_blocks.size();
        m_ap.remove(fd(),cc::Poll::OUT);
        m_lock.unlock();
        while (size > 0)
        {
            if (protocol() == cc::tcp)
            {
                m_lock.lock();
                Block b = m_blocks.front();
                m_blocks.pop_front();
                m_lock.unlock();
                Socket::Send(b, 0);
            }
            else if (protocol() == cc::udp)
            {
                m_lock.lock();
                Block b = m_blocks.front();
                m_blocks.pop_front();
                m_lock.unlock();
                Socket::SendTo(b, m_address, 0);
            }
            m_lock.lock();
            size = m_blocks.size();
            m_lock.unlock();
        }
    }
}

void cc::Connection::Send(const Block &block)
{
    m_lock.lock();
    m_blocks.push_back(block);
    m_ap.add(fd(), cc::Poll::OUT, this);
    m_lock.unlock();
}

void cc::Connection::Close() const
{
    Socket::Close();
}

const cc::Address &cc::Connection::Target() const
{
    return m_address;
}

void cc::Connection::SetDelegate(Delegate *delegate)
{
    m_delegate = delegate;
}