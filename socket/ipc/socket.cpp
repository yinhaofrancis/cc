#include "socket.hpp"

ipc::socket_option operator|(ipc::socket_option v1, ipc::socket_option v2)
{
    return ipc::socket_option((int)v1 | (int)v2);
}
