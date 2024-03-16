#include "pipe.hpp"

cc::Pipe::Pipe()
{
    pipe(m_fd);
}

cc::Pipe::~Pipe() {}

void cc::Pipe::Close()
{
    close(m_fd[0]);
    close(m_fd[1]);
}
int cc::Pipe::rfd()
{
    return m_fd[0];
}

int cc::Pipe::wfd()
{
    return m_fd[1];
}

cc::Fifo::Fifo(std::string &path, mode_t mode) : m_path(path)
{
    mkfifo(path.c_str(), mode);
}

int cc::Fifo::rfd()
{
    if (m_rfd == 0)
    {
        m_rfd = open(m_path.c_str(), O_RDONLY);
    }
    return m_rfd;
}
int cc::Fifo::wfd()
{
    if (m_wfd == 0)
    {
        m_wfd = open(m_path.c_str(), O_WRONLY);
    }
    return m_wfd;
}

void cc::Fifo::Close()
{
    close(m_rfd);
    close(m_wfd);
}