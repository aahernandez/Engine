#include "Engine/Network/LoopBackConnection.hpp"
#include "Engine/Network/NetMessage.hpp"

LoopBackConnection::~LoopBackConnection()
{
	while(!m_messageQueue.empty())
	{
		NetMessage *message = m_messageQueue.front();
		delete message;
		message = nullptr;
		m_messageQueue.pop();
	}
}

void LoopBackConnection::Send(NetMessage *message)
{
	m_messageQueue.push(message);
}

bool LoopBackConnection::Receive(NetMessage **message)
{
	if (m_messageQueue.empty())
	{
		*message = nullptr;
		return false;
	}

	*message = m_messageQueue.front();
	m_messageQueue.pop();

	return true;
}
