#pragma once
#include "Engine/Network/NetAddress.hpp"

class NetSession;
class NetMessage;

class NetConnection
{
public:
	NetSession *m_owner;
	NetAddress_t m_address;
	uint8_t m_connectionIndex;

	virtual ~NetConnection();

	virtual void Send(NetMessage *message) = 0;
	virtual bool Receive(NetMessage **message) = 0;
};