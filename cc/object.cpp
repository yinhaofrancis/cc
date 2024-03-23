#include "object.hpp"

cc::Object::Object()
{
    m_ref_count = new Ref(1);
}
cc::Object::~Object()
{
    this->release();
}
cc::Object::Object(const Object &c)
{
    m_ref_count = c.m_ref_count;
    retain();
}
cc::Object::Object(Object &&c)
{
    m_ref_count = c.m_ref_count;
    c.m_ref_count = nullptr;
}
void cc::Object::operator=(const Object &c)
{
    m_ref_count = c.m_ref_count;
    retain();
}
void cc::Object::operator=(Object &&c)
{
    m_ref_count = c.m_ref_count;
    c.m_ref_count = nullptr;
}
uint64_t cc::Object::refCount()
{
    return m_ref_count->refCount();
}
void cc::Object::retain()
{
    m_ref_count->retain();
}
void cc::Object::release()
{
    if (m_ref_count != nullptr)
    {
        m_ref_count->release();
        if (m_ref_count->refCount() == 0)
        {
            this->dealloc();
            delete m_ref_count;
            m_ref_count = nullptr;
        }
    }
}

void cc::Object::dealloc()
{
}

uint64_t cc::Ref::refCount()
{
    m_mutex.lock();
    auto v = this->value;
    m_mutex.unlock();
    return v;
}

void cc::Ref::release()
{
    if (this->value > 0)
    {
        m_mutex.lock();
        this->value--;
        m_mutex.unlock();
    }
}
void cc::Ref::retain()
{
    m_mutex.lock();
    this->value++;
    m_mutex.unlock();
}

cc::Ref::Ref(uint64_t ref) : value(ref)
{
}