#include "Engine/Network/Net.hpp"
#include "Engine/Network/TcpSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

bool NetSystemStartup()
{
	WORD version = MAKEWORD(2, 2);

	WSADATA data;
	int error = ::WSAStartup(version, &data);
	return (error == 0);
}

void NetSystemShutdown()
{
	::WSACleanup();
}

TCPSocket* CreateListenSocket(unsigned short port)
{
	TCPSocket *listener = new TCPSocket();
	listener->Listen(port);
	listener->SetBlocking(false);
	DebuggerPrintf("Listening on %s", NetAddressToString(listener->m_address).c_str());
	
	return listener;
}
