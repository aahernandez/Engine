#include "Engine/Network/TCPConnection.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

void TCPConnection::Send(NetMessage *message)
{
	unsigned int payloadSize = message->m_payloadBytesUsed + 1;
	m_socket->Send(&payloadSize, 2);
	m_socket->Send(&message->m_messageTypeIndex, 1);
	m_socket->Send(message->m_payload, message->m_payloadBytesUsed);
}

bool TCPConnection::Receive(NetMessage **message)
{
	NetMessage *netMessage = nullptr;

	if (m_bufferOffset < 2)
	{
		m_bufferOffset += m_socket->Receive(&m_payloadBuffer[m_bufferOffset], 2 - m_bufferOffset);
	}
	if (m_bufferOffset < 2)
	{
		return false;
	}

	unsigned int payloadNeeded = (*(uint16_t*)m_payloadBuffer) + 2;
	
	if (m_bufferOffset < payloadNeeded)
	{
		m_bufferOffset += m_socket->Receive(&m_payloadBuffer[m_bufferOffset], payloadNeeded - m_bufferOffset);
	}
	if (m_bufferOffset < payloadNeeded)
	{
		return false;
	}
	else
	{
		*message = new NetMessage();
		netMessage = *message;

		netMessage->m_payloadBytesUsed = payloadNeeded - 3;
		ASSERT_OR_DIE(netMessage->m_payloadBytesUsed < maxPayloadSize, "Payload bytes sent is past the max payload size limit.");

		netMessage->m_messageTypeIndex = (byte_t)m_payloadBuffer[2];

		for (unsigned int index = 0; index < netMessage->m_payloadBytesUsed; index++)
		{
			netMessage->m_payload[index] = (byte_t)m_payloadBuffer[index + 3];
		}

		netMessage->m_payload[netMessage->m_payloadBytesUsed] = NULL;

		m_bufferOffset = 0;
		return true;
	}
}

bool TCPConnection::Connect()
{
	return m_socket->Join(m_address);
}

bool TCPConnection::IsDisconnected()
{
	if (!m_socket->IsValid() || m_socket == nullptr)
		return true;
	return false;
}
