#ifndef BLOCK_HPP
#define BLOCK_HPP
#include<memory>
#include"ref.hpp" 
namespace cc
{
    class Block:Ref{
    public:
        Block(const void* buffer, const size_t size);
        Block(const size_t size);
        Block(std::string& string);
        Block(std::string&& string);
        const void* data() const;
        const size_t size() const;
        void append(const Block&);
        void assign(const void* buffer,const size_t size);
        std::string str() const;
        const char* c_str() const;
        friend class Stream;
        friend class Socket;
    private:
        void* m_block = nullptr;
        size_t m_size = 0;
    };
} // namespace cc


#endif   