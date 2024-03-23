#ifndef OBJCECT_HPP
#define OBJCECT_HPP
#include <atomic>
#include <unistd.h>
#include <mutex>
namespace cc
{
    class Ref
    {
    public:
       uint64_t refCount();
       void release();
       void retain();
       Ref(uint64_t ref);
    private:
        std::atomic_uint64_t value;
    };

    class Object{
    public:
        Object();
        virtual ~Object();
        Object(const Object&);
        Object(Object&&);  
        void operator=(const Object&);
        void operator=(Object&&); 
        uint64_t refCount();
    protected:
        virtual void dealloc();
        void retain();
        void release();
    private:
        Ref** m_ref_count = nullptr;
    };
}

#endif