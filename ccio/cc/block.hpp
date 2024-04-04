#ifndef BLOCK_HPP
#define BLOCK_HPP
#include<memory>
namespace cc
{
    class Block{
    public:
        Block(const void* buffer, const size_t size);
        Block(const size_t size);
        Block(std::string& string);
        Block(std::string&& string);
        Block copy() const;
        const void* data() const;
        const size_t size() const;
        void append(const Block&);
        void assign(const void* buffer,const size_t size);
        void Free();
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