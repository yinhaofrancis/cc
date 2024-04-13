#include "define.hpp"

rpc::status rpc::operator|(rpc::status v1, rpc::status v2)
{
    return rpc::status(int(v1) | int(v2));
}
