
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "define.hpp"

namespace ipc
{   
     enum socket_option{
        reuse_addr  =   SO_REUSEADDR,
        keep_alive  =   SO_KEEPALIVE,
        reuse_port  =   SO_REUSEPORT,
        liger       =   SO_LINGER,
        snd_buf     =   SO_SNDBUF,
        rcv_buf     =   SO_RCVBUF,
        rcv_lowat   =   SO_RCVLOWAT,
        snd_lowat   =   SO_SNDLOWAT
    };

    template<domain d,sock s,protocol p>  
    class socket:public stream {
    public:
        socket():stream(::socket(d,s,p)) { };
        int bind(address<d> addr) const{
            return ::bind(fd,addr.raw(),sizeof(addr));
        }
        int listen(int backlog) const{
            return ::listen(fd,backlog);
        }
        int connect(address<d> addr) const{
            return connect(fd,addr.raw(),addr.size());
        }
        int sendto(address<d> addr,const void* buffer,size_t len,int flags) const{
            static_assert(s == dgram,"sock is not dgram");
            return ::sendto(fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int recvfrom(address<d> addr,void* buffer,size_t len,int flags) const{
            static_assert(s == dgram,"sock is not dgram");
            return ::recvfrom(fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int send(void* buffer,size_t len,int flags) const{
            static_assert(s == stream,"sock is not stream");
            return ::send(fd,buffer,len,flags);
        }
        int recv(void* buffer,size_t len,int flags) const{
            static_assert(s == stream,"sock is not stream");
            return ::recv(fd,buffer,len,flags);
        }
        int setOption(socket_option op,int flag){
            return setsockopt(fd,SOL_SOCKET,op,&flag,sizeof(flag));
        }
        const domain m_domain = d;
        const sock m_sock = s;
        const protocol m_protocol = p;
    };
} // namespace ipc

#endif

