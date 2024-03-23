#ifndef REF_HPP
#define REF_HPP
#include <atomic>
#include <mutex>
namespace cc
{
    class RefCount{

       RefCount(uint64_t count);
       RefCount(RefCount & rc);
       RefCount(RefCount && rc); 
       void operator=(RefCount & rc);
       void operator=(RefCount && rc);
       void release();
       void retain();
    private:
       uint64_t ** m_count;
       std::mutex *m_mutex;
    };
}



#endif