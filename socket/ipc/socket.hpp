
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "define.hpp"
#include "stream.hpp"

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

    socket_option operator | (socket_option v1 ,socket_option v2);

    template<domain d,sock s,protocol p>  
    class socket:public stream {
    public:
        socket():stream(::socket(d,s,p)) { };

        socket(int fd):stream(fd) { };

        int bind(address<d> addr) const{
            return ::bind(stream::fd,addr.raw(),addr.size());
        }
        int listen(int backlog) const{
            return ::listen(stream::fd,backlog);
        }
        int connect(address<d> addr) const{
            return ::connect(stream::fd,addr.raw(),addr.size());
        }
        int sendto(address<d> addr,const void* buffer,size_t len,int flags) const{;
            return ::sendto(stream::fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int recvfrom(address<d> addr,void* buffer,size_t len,int flags) const{
            return ::recvfrom(stream::fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int send(const void* buffer,size_t len) const{
            return stream::send(buffer,len);
        }
        ipc::stream accept(address<d> &addr) const {
            socklen_t size;
            int fd = ::accept(stream::fd,(sockaddr*)addr.raw(),&size);
            return ipc::stream(fd);
        }
        int recv(void* buffer,size_t len,int flags) const{
            return stream::recv(stream::fd,buffer,len);
        }
        int setOption(socket_option op,int flag) const{
            return setsockopt(stream::fd,SOL_SOCKET,op,&flag,sizeof(flag));
        }
        const domain m_domain = d;
        const sock m_sock = s;
        const protocol m_protocol = p;
        
    };
} // namespace ipc

#endif

