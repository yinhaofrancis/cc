#ifndef REF_HPP
#define REF_HPP
#include <mutex>
#include <unistd.h>
#include <functional>
#include <vector>
namespace cc
{
    class Ref
    {
        struct Count{
            uint64_t count;
            std::vector<std::function<void()>> m_dealloc;
            Count(uint64_t count);
        };
    public:
        Ref();
        virtual ~Ref();
        Ref(const Ref&);
        Ref(const Ref&&);
        void operator=(const Ref&);
        void operator=(const Ref&&);
        u_int64_t count();
    protected:
        void dealloc(std::function<void()> dealloc);
    private:
        void inline release();
        void inline retain();
        std::mutex m_lock;
        Count** m_count;
        
    };
}

#endif