#include "define.hpp"

ipc::status ipc::operator|(ipc::status v1, ipc::status v2)
{
    return ipc::status(int(v1) | int(v2));
}
