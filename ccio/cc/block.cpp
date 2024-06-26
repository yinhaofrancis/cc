#include "block.hpp"
#include <cstring>
#include <string>
#include <cmath>
#include <stdlib.h>
cc::Block::Block(const void *buffer, const size_t size):Block(size)
{
    std::memcpy(m_block,buffer,size);   
}

cc::Block::Block(const size_t size):m_size(size),m_block(malloc(size + 1))
{
    std::memset(m_block,0,size + 1);
}

cc::Block::Block(std::string& string):Block(string.data(),string.size()){}

cc::Block::Block(std::string &&string):Block(string.data(),string.size()){}

cc::Block cc::Block::copy() const
{
    return cc::Block(this->m_block,this->m_size);
}

const void *cc::Block::data() const
{
    return m_block;
}

const size_t cc::Block::size() const
{
    return m_size;
}

void cc::Block::append(const Block & block)
{
    size_t continusP = this->size();
    this->m_size += block.m_size;

    m_block = realloc(m_block,this->size() + 1);
    std::memcpy(reinterpret_cast<char*>(m_block) + continusP ,block.m_block,block.m_size + 1);
}

void cc::Block::assign(const void *buffer, const size_t size)
{
    
    std::memcpy(m_block,buffer,std::min(size,m_size));
}

void cc::Block::Free()
{
    if(m_block != nullptr){
        free(m_block);
        m_block = nullptr;
    }
}

std::string cc::Block::str() const
{
    return c_str();
}

const char *cc::Block::c_str() const 
{
    return static_cast<const char*>(m_block);
}
