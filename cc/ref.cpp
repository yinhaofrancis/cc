#include "ref.hpp"

void cc::RefCount::operator=(RefCount &rc)
{
}
void cc::RefCount::operator=(RefCount &&rc)
{
}
void cc::RefCount::release()
{
    m_mutex->lock();
    if(m_count != nullptr && *m_count != nullptr){
        **m_count--;
        if(**m_count == 0){
            delete *m_count;
            delete m_count;
            m_count = nullptr;
        }
    }
    m_mutex->unlock();
}
void cc::RefCount::retain()
{
    m_mutex->lock();
    if(m_count != nullptr && *m_count != nullptr){
        **m_count++;
    }
    m_mutex->unlock();
}
cc::RefCount::RefCount(uint64_t count)
{
    m_count = new uint64_t*[1];
    *m_count = new uint64_t(count);
    m_mutex = new std::mutex();
}

cc::RefCount::RefCount(RefCount &rc)
{

}
cc::RefCount::RefCount(RefCount &&rc)
{

}