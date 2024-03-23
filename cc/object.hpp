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
        uint64_t value;
        std::mutex m_mutex;
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
        Ref* m_ref_count;
    };
}

#endif