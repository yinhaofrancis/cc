#include "http.hpp"

void cc::http_server_delegate::onConnect(TCPServer &server, Client &fd, Address &address)
{
}

void cc::http_server_delegate::onDisconnect(TCPServer &server, Client &fd, const Address &address, const char *msg)
{
}

void cc::http_server_delegate::onRead(TCPServer &server, Client &fd, const Address &address, Block &block)
{
}

void cc::http_server_delegate::onWrite(TCPServer &server, Client &fd, const Address &address)
{
}
