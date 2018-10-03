#pragma once
#include "Engine/Network/NetConnection.hpp"
#include "Engine/Network/NetMessage.hpp"

class TCPSocket;

class TCPConnection : public NetConnection
{
public:
	TCPSocket *m_socket;
	byte_t m_payloadBuffer[maxPayloadSize];
	unsigned int m_bufferOffset = 0;

	virtual void Send(NetMessage *message) override;
	virtual bool Receive(NetMessage **message) override;
	bool Connect();

	bool IsDisconnected();
};