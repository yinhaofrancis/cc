#include "socket.hpp"

rpc::socket_option operator|(rpc::socket_option v1, rpc::socket_option v2)
{
    return rpc::socket_option((int)v1 | (int)v2);
}
