#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include<sys/event.h> 
#include<sys/types.h>
#include<vector>
#include<array>

namespace rpc
{
    enum event_type {
        read = EVFILT_READ,
        write = EVFILT_WRITE,
        timer = EVFILT_TIMER,
        signal = EVFILT_SIGNAL,
    };
    enum event_op{
        add = EV_ADD,
        remove = EV_DELETE,
        enable = EV_ENABLE,
        disable = EV_DISABLE,
        clear = EV_CLEAR,
        err = EV_ERROR,
        dispatch = EV_DISPATCH,
        receipt = EV_RECEIPT,
    };
    event_op operator|(event_op lhs,event_op rhs){
        return (event_op)((int)lhs | (int)rhs);
    }
   
    enum event_note{
        none_note = 0,
        seconds = NOTE_SECONDS,
        millseconds = 0,
        microseconds = NOTE_USECONDS,
        nanoseconds = NOTE_NSECONDS,
        absolute = NOTE_ABSOLUTE,
    };

    event_note operator|(event_note lhs,event_note rhs){
        return (event_note)((int)lhs | (int)rhs);
    }
    class event{
    public:
        event(int ident,event_op op,event_type et,event_note note,int64_t data)
        :ident(ident)
        ,op(op)
        ,et(et)
        ,note(note)
        ,data(data) {};

        event(struct kevent& event)
        :ident(event.ident)
        ,op((event_op)event.flags)
        ,et((event_type)event.filter)
        ,note((event_note)event.fflags)
        ,data(event.data){

        }
        void mount(int kq){
            struct kevent change;
            EV_SET(&change,ident,et,op,note,data,0);
            kevent(kq,&change,1,nullptr,0,nullptr);
        }
        bool occur_error(){
            return op & EV_ERROR;
        }
        template<int max>
        static int wait(int kq,timespec* timeout,std::vector<event>& result) {
            std::array<struct kevent,max> out;
            int c = kevent(kq,nullptr,0,out.data(),out.size(),timeout);
            for (size_t i = 0; i < c; i++)
            {
                result.push_back(event(out[i]));
            }
            
            return c;
        };
        const int ident;
        const event_type et;
        const event_note note;
        const int64_t data;
    private:
        event_op op;

    };
} // namespace rpc




#endif


