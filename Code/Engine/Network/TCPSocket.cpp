#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

TCPSocket::TCPSocket()
	: m_socket(INVALID_SOCKET)
	, m_isListenSocket(false)
{
}

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Join(NetAddress_t addr)
{
	if (IsValid())
	{
		return false;
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage internalAddress;
	int addrSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&internalAddress, &addrSize, addr);

	int result = ::connect(sock, (sockaddr*) &internalAddress, addrSize);
	if (result != 0)
	{
		::closesocket(sock);
		return false;
	}
	else
	{
		m_socket = sock;
		m_address = addr;
		return true;
	}
}

bool TCPSocket::Join(std::string addrStr)
{
	std::vector<std::string> addressAndPort = ParseStringIntoPiecesByDelimiter(addrStr, ":");
	int port = std::stoi(addressAndPort[1]);
	std::vector<NetAddress_t> addresses = GetAddressesFromHostName(addressAndPort[0].c_str(), (short)port, false);
	return Join(addresses[0]);
}

bool TCPSocket::IsValid() const
{
	return (m_socket != INVALID_SOCKET);
}

void TCPSocket::Close()
{
	if (IsValid())
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		m_isListenSocket = false;
	}
}

unsigned int TCPSocket::Send(void const *payload, unsigned int payloadSizeInBytes)
{
	if (!IsValid())
	{
		return 0;
	}

	if (IsListenSocket())
	{
		return 0;
	}

	if (payloadSizeInBytes == 0)
	{
		return 0;
	}

	int bytesSent = ::send(m_socket, (char const*)payload, (int)payloadSizeInBytes, 0);
	if (bytesSent <= 0)
	{
		int error = WSAGetLastError();
		DebuggerPrintf("TCPSocket::send failed. Error %u", error);
		Close();
		return 0;
	}

	ASSERT_OR_DIE((unsigned int)bytesSent == payloadSizeInBytes, "The bytes sent was a different size than the payload size.");
	return bytesSent;
}

unsigned int TCPSocket::Receive(void *payload, int maxPayloadSize)
{
	if (!IsValid() || maxPayloadSize == 0)
	{
		return 0;
	}

	if (IsListenSocket())
	{
		return 0;
	}

	ASSERT_OR_DIE(payload != nullptr, "The received payload was nullptr.");

	int bytesRead = ::recv(m_socket, (char*)payload, (int)maxPayloadSize, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				return 0;
			}
			else
			{
				Close();
			}
		}
		else
		{
			CheckForDisconnect();
			return 0;
		}
		return 0;
	}	
	else
	{
		return (unsigned int)bytesRead;
	}
}

void TCPSocket::SetBlocking(bool isBlocking)
{
	if (!IsValid())
	{
		return;
	}

	unsigned long nonBlocking = isBlocking ? 0 : 1;
	::ioctlsocket(m_socket, FIONBIO, &nonBlocking);
}

void TCPSocket::CheckForDisconnect()
{
	if (!IsValid())
	{
		return;
	}

	WSAPOLLFD fd;
	fd.fd = m_socket;
	fd.events = POLLRDNORM;

	if (SOCKET_ERROR == WSAPoll(&fd, 1, 0))
	{
		return;
	}

	if ((fd.revents & POLLHUP) != 0)
	{
		Close();
	}
}

bool TCPSocket::Listen(uint16_t port)
{
	if (IsValid())
	{
		false;
	}

	std::vector<NetAddress_t> addresses = GetAddressesFromHostName("", port, true);
	if (addresses.empty())
	{
		return false;
	}

	SOCKET listenSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage listenAddress;
	int addrSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&listenAddress, &addrSize, addresses[0]);

	//Associate address to this socket
	int result = ::bind(listenSocket, (sockaddr*)&listenAddress, (int)addrSize);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listenSocket);
		return false;
	}

	int maxQueued = 8;
	result = ::listen(listenSocket, (int)maxQueued);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listenSocket);
		return false;
	}

	m_socket = listenSocket;
	m_address = addresses[0];
	m_isListenSocket = true;
	return true;
}

bool TCPSocket::IsListenSocket() const
{
	return m_isListenSocket;
}

TCPSocket* TCPSocket::Accept()
{
	if (!IsListenSocket())
	{
		return nullptr;
	}

	sockaddr_storage connAddr;
	int connAddrLen = sizeof(connAddr);

	SOCKET theirSocket = ::accept(m_socket, (sockaddr*)&connAddr, &connAddrLen);
	if (theirSocket == INVALID_SOCKET) 
	{
		return nullptr;
	}

	NetAddress_t netAddr;
	if (!NetAddressFromSocketAddress(&netAddr, (sockaddr*)&connAddr))
	{
		::closesocket(theirSocket);
		return nullptr;
	}
	
	TCPSocket *theirTCPSocket = new TCPSocket();
	theirTCPSocket->m_socket = theirSocket;
	theirTCPSocket->m_address = netAddr;

	return theirTCPSocket;
}

