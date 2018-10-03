#pragma once
#include "Engine/Network/NetConnection.hpp"
#include <queue>

class LoopBackConnection : public NetConnection
{
public:
	std::queue<NetMessage*> m_messageQueue;

	virtual ~LoopBackConnection();

	virtual void Send(NetMessage *msg) override;
	virtual bool Receive(NetMessage **msg) override;
};