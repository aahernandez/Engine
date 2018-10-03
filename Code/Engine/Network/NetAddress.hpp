#pragma once
#include <vector>

struct sockaddr;

struct NetAddress_t
{
	unsigned short port;
	unsigned int address;
};

std::vector<NetAddress_t> GetAddressesFromHostName(char const *hostName, unsigned short port, bool bindable);
NetAddress_t GetMyAddress(unsigned short port = 0);
bool NetAddressFromSocketAddress(NetAddress_t *out, sockaddr *socketAddress);
void SocketAddressFromNetAddress(sockaddr* out, int *outSize, NetAddress_t const &addr);
std::string NetAddressToString(const NetAddress_t &addr);