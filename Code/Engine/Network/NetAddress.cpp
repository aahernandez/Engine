#include "Engine/Network/NetAddress.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

std::vector<NetAddress_t> GetAddressesFromHostName(char const *hostName, unsigned short port, bool bindable)
{
	char service[16];
	sprintf_s(service, "%u", port);

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //udp = SOCK_DGRAM
	hints.ai_flags |= (bindable ? AI_PASSIVE : 0);
	// hints.ai_flags |= AI_NUMERICHOST; //don't use this if you're using strings for the host name like "google.com"


	addrinfo *results = nullptr;
	int status = ::getaddrinfo(hostName, service, &hints, &results);

	std::vector<NetAddress_t> addresses;
	if (status != 0)
	{
		return addresses;
	}

	addrinfo *addr = results;
	while (addr != nullptr)
	{
		NetAddress_t netAddr;
		if (NetAddressFromSocketAddress(&netAddr, addr->ai_addr))
		{
			addresses.push_back(netAddr);
		}
		addr = addr->ai_next;
	}

	::freeaddrinfo(addr);
	return addresses;
}

NetAddress_t GetMyAddress(unsigned short port)
{
	std::vector<NetAddress_t> addresses = GetAddressesFromHostName("", port, false);
	return addresses[0];
}

bool NetAddressFromSocketAddress(NetAddress_t *out, sockaddr *socketAddress)
{
	if (socketAddress == nullptr)
	{
		return false;
	}

	if (socketAddress->sa_family != AF_INET)
	{
		return false;
	}

	sockaddr_in* ipv4_address = (sockaddr_in*)socketAddress;

	out->port = ntohs(ipv4_address->sin_port);
	out->address = ntohl(ipv4_address->sin_addr.S_un.S_addr);

	return true;
}

void SocketAddressFromNetAddress(sockaddr* out, int *outSize, NetAddress_t const &addr)
{
	sockaddr_in *outAddr = (sockaddr_in*)out;
	memset(outAddr, 0, sizeof(sockaddr_in));
	outAddr->sin_family = AF_INET;
	outAddr->sin_port = htons(addr.port);
	outAddr->sin_addr.S_un.S_addr = htonl(addr.address);

	*outSize = sizeof(sockaddr_in);
}

std::string NetAddressToString(const NetAddress_t &addr)
{
	unsigned char *arr = (unsigned char*)&addr.address;

	unsigned char a = arr[0];
	unsigned char b = arr[1];
	unsigned char c = arr[2];
	unsigned char d = arr[3];

	std::string stringAddr = std::to_string(d) + "." + std::to_string(c) + "." + std::to_string(b) + "." + std::to_string(a) + ":" + std::to_string(addr.port);
	return stringAddr;
}
