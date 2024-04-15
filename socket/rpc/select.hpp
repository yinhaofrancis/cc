#ifndef SELECT_HPP
#define SELECT_HPP
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/event.h>
#include <sys/types.h>
#include <vector>
#include <thread>

namespace rpc
{

    enum select_event
    {
        se_in,
        se_out,
        se_err
    };
    template <select_event se,typename FD>
    class select
    {
    public:
        select(timeval time, std::function<int(FD&&)> callback)
        {
            bool* c_running = new bool(true);
            std::mutex* c_lock = new std::mutex();
            std::vector<int> * c_fds = new std::vector<int>();
            m_running = c_running;
            m_lock = c_lock;
            m_FDs = c_fds;
            std::thread([this,c_running,c_lock,c_fds,callback,time](){
                while (*c_running)
                {
                    fd_set m_sets;
                    c_lock->lock();
                    auto maxfd = this->load_set(*c_fds,&m_sets);
                    c_lock->unlock();
                    int count;
                    if(se == se_out){
                        count = ::select(maxfd + 1,nullptr,&m_sets,nullptr,(struct timeval *)&time);
                    }else if(se == se_err){
                        count = ::select(maxfd + 1,nullptr,nullptr,&m_sets,(struct timeval *)&time);
                    }else{
                        count = ::select(maxfd + 1,&m_sets,nullptr,nullptr,(struct timeval *)&time);
                    }
                    if(count > 0){
                        std::vector<int> current;
                        
                        for (auto i :(*c_fds)){
                            if (FD_ISSET(i,&m_sets)){
                                int ret = callback(FD(i));
                                if(ret > 0){
                                    current.push_back(i);
                                }
                            }else{
                                current.push_back(i);
                            }
                            c_lock->lock();
                            *c_fds = current;
                            c_lock->unlock();
                        }

                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                c_lock->lock();
                delete c_running;
                this->m_running = nullptr;
                delete c_fds;
                this->m_FDs = nullptr;
                c_lock->unlock();
                this->m_lock = nullptr;
                delete c_lock;
            }).detach();
        }

        void end()
        {
            *m_running = false;
        }
        
        void add(int fd){
            if(m_running != nullptr && *m_running == true){
                m_lock->lock();
                m_FDs->push_back(fd);
                m_lock->unlock();
            }
            
        }

    private:
        bool* m_running = nullptr;

        std::mutex * m_lock = nullptr;

        std::vector<int> *m_FDs;

        int load_set(std::vector<int>&fds,fd_set* set){
            int max = 0; 
            FD_ZERO(set);        
            for (auto &&i:fds){
                max = std::max(max,i);
                FD_SET(i,set);
            }
            return max;
        }
    };
} // namespace rpc

#endif