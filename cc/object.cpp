#include "object.hpp"

cc::Object::Object()
{

    m_ref_count = new Ref*[1];
    *m_ref_count = new Ref(1);
}
cc::Object::~Object()
{
    this->release();
    delete m_ref_count;
    m_ref_count = nullptr;
}
cc::Object::Object(const Object &c)
{
    m_ref_count = new Ref*[1];
    *m_ref_count = (*c.m_ref_count);
    (*c.m_ref_count)->retain();
}
cc::Object::Object(Object &&c)
{
    m_ref_count = new Ref*[1];
    *m_ref_count = (*c.m_ref_count);
    (*c.m_ref_count)->retain();
}
void cc::Object::operator=(const Object &c)
{
    if (m_ref_count != nullptr && *m_ref_count != nullptr)
    {
        (*m_ref_count)->release();
    }
    *m_ref_count = *c.m_ref_count;
    (*c.m_ref_count)->retain();
}
void cc::Object::operator=(Object &&c)
{
    if (m_ref_count != nullptr && *m_ref_count != nullptr)
    {
        (*m_ref_count)->release();
    }
    *m_ref_count = *c.m_ref_count;
    (*c.m_ref_count)->retain();
}
uint64_t cc::Object::refCount()
{
    return (*m_ref_count)->refCount();
}
void cc::Object::retain()
{
    (*m_ref_count)->retain();
}
void cc::Object::release()
{
    if (m_ref_count != nullptr && *m_ref_count != nullptr)
    {
        (*m_ref_count)->release();
        if ((*m_ref_count)->refCount() == 0)
        {
            this->dealloc();
        }
    }
}

void cc::Object::dealloc()
{
    delete *m_ref_count;
    *m_ref_count = nullptr;
}

uint64_t cc::Ref::refCount()
{
    return this->value;
}

void cc::Ref::release()
{
    if (this->value > 0)
    { 
        this->value--;
    }
}
void cc::Ref::retain()
{
    this->value++;
}

cc::Ref::Ref(uint64_t ref) : value(ref){}