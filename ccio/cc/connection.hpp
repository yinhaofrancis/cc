
#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__
#include "socket.hpp"
#include "poll.hpp"
#include "task.hpp"
#include "poll.hpp"
namespace cc
{
    // class Connection : protected Socket,public AsyncPoll::AsyncPollCallback
    // {
    //     class Delegate
    //     {
    //     public:
    //         virtual void onRead(const Connection& connect, Block &block){};
    //         virtual void onWrite(const Connection& connect){};
    //     };
    //     enum ConnectState{
    //         Closed,
    //         Connecting,
    //         Connected
    //     };

    // public:
    //     Connection(Domain af, SockType sock, Protocol proto);
    //     Connection(const Connection &) = delete;
    //     Connection(const Connection &&) = delete;
    //     int setTarget(const Address &address);
    //     void SetDelegate(Delegate *delegate);
    //     void SetPoll(AsyncPoll &poll) const;
    //     void Send(const Block& block) const;
    //     void Recieve(Block &block);
    //     void Close();
    //     Address Target();
    //     ConnectState State();
    // protected:
    //     virtual void onEvent(AsyncPoll &poll,Poll::Result&);
    // private:
    //     Delegate * m_delegate = nullptr;
    //     Address m_target_address;
    //     ConnectState m_state;
    // };

    class Connection : protected Socket,protected AsyncPoll::AsyncPollCallback
    {
        class Delegate
        {
        public:
            virtual void onConnect(const Connection& connect){};
            virtual void onDisconnect(const Connection& connect){};
            virtual void onRead(const Connection& connect, Block &block){};
            virtual void onRead(const Connection& connect, Block &block,Address &addr) {}
        };
    public:
        Connection(Domain domain,SockType sock, Protocol proto,Address &address);
        Connection(const Connection &) = delete;
        Connection(const Connection &&) = delete;
        virtual void onEvent(AsyncPoll &poll,cc::Poll::Result& result);
        void Send(const Block &block);
        void Close() const;
        const Address& Target() const;
        void SetDelegate(Delegate* delegate);
    private:
        std::mutex m_lock;
        Address m_address;
        AsyncPoll m_ap;
        std::deque<Block> m_blocks;
        Delegate* m_delegate = nullptr;

    };
} // namespace cc

#endif