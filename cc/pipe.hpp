#ifndef PIPE_HPP
#define PIPE_HPP
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
namespace cc
{
    class Pipe
    {
    public:
        Pipe();
        ~Pipe();
        void Close();
        int wfd();
        int rfd();
    private:
        int m_fd[2];
    };

    class Fifo{
    public:
        Fifo(std::string &path,mode_t mode);
        void Close();
        int rfd();
        int wfd();
    private:
        int m_rfd = 0;
        int m_wfd = 0;
        std::string m_path;
    };

} 

#endif