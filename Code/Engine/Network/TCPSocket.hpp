#pragma once
#include "Engine/Network/NetAddress.hpp"

typedef _W64 unsigned int UINT_PTR;
typedef UINT_PTR SOCKET;

const int MAX_CONNECTIONS = 20;

class TCPSocket
{
	public:
		SOCKET m_socket;
		NetAddress_t m_address;
		bool m_isListenSocket;

		TCPSocket();
		~TCPSocket();

		//CLIENT
		bool Join(NetAddress_t addr);
		bool Join(std::string addrStr);


		bool IsValid() const;

		void Close();

		//HOST
		bool Host(uint16_t port);

		unsigned int Send(void const *payload, unsigned int payloadSizeInBytes);
		unsigned int Receive(void *payload, int maxPayloadSize);

		void SetBlocking(bool blocking);
		void CheckForDisconnect();

		bool Listen(uint16_t port);
		bool IsListenSocket() const;
		TCPSocket* Accept();
};