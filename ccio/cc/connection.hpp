
#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__
#include "socket.hpp"
#include "poll.hpp"
#include "task.hpp"
#include "poll.hpp"
namespace cc
{
    class Connection : protected Socket,public AsyncPoll::AsyncPollCallback
    {
        class Delegate
        {
        public:
            virtual void onRead(const Connection& connect, Block &block){};
            virtual void onWrite(const Connection& connect){};
        };
        enum ConnectState{
            Closed,
            Connecting,
            Connected
        };

    public:
        Connection(Domain af, SockType sock, Protocol proto);
        Connection(const Connection &) = delete;
        Connection(const Connection &&) = delete;
        int setTarget(const Address &address);
        void SetDelegate(Delegate *delegate);
        void SetPoll(AsyncPoll &poll) const;
        void Send(const Block& block) const;
        void Recieve(Block &block);
        void Close();
        Address Target();
        ConnectState State();
    protected:
        virtual void onEvent(AsyncPoll &poll,Poll::Result&);
    private:
        Delegate * m_delegate = nullptr;
        Address m_target_address;
        ConnectState m_state;
    };
} // namespace cc




#endif