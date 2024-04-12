
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "define.hpp"

namespace ipc
{   
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
            return ::sendto(fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int recvfrom(address<d> addr,void* buffer,size_t len,int flags) const{
            return ::recvfrom(fd,buffer,len,flags,addr.raw(),addr.size());
        }
        int send(void* buffer,size_t len,int flags) const{
            return ::send(fd,buffer,len,flags);
        }
        int recv(void* buffer,size_t len,int flags) const{
            return ::recv(fd,buffer,len,flags);
        }
        const domain m_domain = d;
        const sock m_sock = s;
        const protocol m_protocol = p;
    };
} // namespace ipc

#endif

