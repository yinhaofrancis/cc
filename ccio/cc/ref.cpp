#include "ref.hpp"


cc::Ref::Ref():m_count(new cc::Ref::Count*(new cc::Ref::Count(1))){
}

cc::Ref::~Ref()
{
    release();
}

cc::Ref::Ref(const Ref & r):m_count(new cc::Ref::Count*())
{
    if (r.m_count == nullptr)
        return;
    *m_count = *r.m_count;
    retain();
}

cc::Ref::Ref(const Ref && r):m_count(new cc::Ref::Count*())
{
    if (r.m_count == nullptr)
        return;
    *m_count = *r.m_count;
    retain();
}

void cc::Ref::operator=(const Ref & r)
{
    if (r.m_count == nullptr)
        return;
    if(*this->m_count != nullptr){
        release();
    }
    m_count = new cc::Ref::Count*();
    *m_count = *r.m_count;
    retain();
}

void cc::Ref::operator=(const Ref && r)
{
    if (r.m_count == nullptr)
        return;
    if(*this->m_count != nullptr){
        release();
    }
    m_count = new cc::Ref::Count*();
    *m_count = *r.m_count;
    retain();
}

u_int64_t cc::Ref::count()
{
    if(m_count != nullptr && *m_count != nullptr){
        return (*m_count)->count;
    }
    return 0;
}



void cc::Ref::dealloc(std::function<void()> dealloc)
{
    (*this->m_count)->m_dealloc.push_back(dealloc);
}

inline void cc::Ref::release()
{
    m_lock.lock();
    if(m_count != nullptr && *m_count != nullptr)
        (**m_count).count--;
    m_lock.unlock();
    if((**m_count).count == 0){
        for (auto &&i : (*this->m_count)->m_dealloc)
        {
            i();
        }
        (*this->m_count)->m_dealloc.clear();
        delete  *m_count;
        *m_count = nullptr;
    }
    delete m_count;
    m_count = nullptr;
}

inline void cc::Ref::retain()
{
    m_lock.lock();
    if(m_count != nullptr && *m_count != nullptr)
        (**m_count).count++;
    m_lock.unlock();
}

cc::Ref::Count::Count(uint64_t count):count(count)
{
    
}
