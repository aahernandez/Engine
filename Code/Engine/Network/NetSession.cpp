#include "Engine/Network/NetSession.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/NetMessageDefinition.hpp"
#include "Engine/Network/NetConnection.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

bool NetSession::RegisterMessageDefinition(uint8_t messageID, NetMessageDefinition &definition)
{
	if (messageID >= (uint8_t)m_messageDefinitions.size())
		m_messageDefinitions.resize(messageID + 1);
	m_messageDefinitions[messageID] = &definition;
	return true;
}

bool NetSession::RegisterMessageFunction(uint8_t messageID, std::function<void(NetMessage*)> callback)
{
	NetMessageDefinition *messageDef = new NetMessageDefinition();
	messageDef->m_typeIndex = messageID;
	messageDef->m_handler = callback;

	return RegisterMessageDefinition(messageID, *messageDef);
}

NetMessageDefinition* NetSession::GetMessageDefinition(uint8_t id) const
{
	return m_messageDefinitions.at(id);
}

uint8_t NetSession::GetFreeConnectionIndex() const
{
	unsigned int index = 0;
	for (index = 0; index < (unsigned int)m_connections.size(); ++index) 
	{
		if (m_connections[index] == nullptr) 
		{
			return (uint8_t)index;
		}
	}

	if (index < m_maxConnectionCount) 
	{
		return (uint8_t)index;
	}
	else 
	{
		return INVALID_CONNECTION_INDEX;
	}
}

void NetSession::JoinConnection(uint8_t index, NetConnection *connection)
{
	connection->m_connectionIndex = index;

	ASSERT_OR_DIE((index >= m_connections.size()) || (m_connections[index] == nullptr), "Connection index not valid.");

	if (index >= m_connections.size()) 
	{
		m_connections.resize(index + 1);
	}

	m_connections[index] = connection;
}

void NetSession::DestroyConnection(NetConnection *netConnection)
{
	if (netConnection == nullptr) 
	{
		return;
	}

	if (m_myConnection == netConnection) 
	{
		m_myConnection = nullptr;
	}

	if (m_hostConnection == netConnection) 
	{
		m_hostConnection = nullptr;
	}

	if (netConnection->m_connectionIndex != INVALID_CONNECTION_INDEX) 
	{
		m_connections[netConnection->m_connectionIndex] = nullptr;
		netConnection->m_connectionIndex= INVALID_CONNECTION_INDEX;
	}

	delete netConnection;
}

NetConnection* NetSession::GetConnection(uint8_t index)
{
	if (index < m_connections.size()) {
		return m_connections[index];
	}

	return nullptr;
}

void NetSession::SendMessageToOthers(NetMessage const &message)
{
	std::vector<NetConnection*>::iterator connIter;
	for (connIter = m_connections.begin(); connIter != m_connections.end(); ++connIter)
	{
		NetConnection *netConnection = *connIter;
		if (netConnection != m_myConnection && netConnection != nullptr)
		{
			netConnection->Send(new NetMessage(message));
		}
	}
}

void NetSession::SendMessageToIndex(int index, NetMessage const &message)
{
	m_connections[index]->Send(new NetMessage(message));
}

void NetSession::SendMessageToAll(NetMessage const &message)
{
	m_myConnection->Send(new NetMessage(message));
	SendMessageToOthers(message);
}

void NetSession::SendMessageToHost(NetMessage const &message)
{
	m_hostConnection->Send(new NetMessage(message));
}

void NetSession::SendMessageToOthersButOne(NetMessage const &message, NetConnection *connection)
{
	std::vector<NetConnection*>::iterator connIter;
	for (connIter = m_connections.begin(); connIter != m_connections.end(); ++connIter)
	{
		NetConnection *netConnection = *connIter;
		if (netConnection != nullptr && netConnection != m_myConnection && netConnection != connection)
		{
			netConnection->Send(new NetMessage(message));
		}
	}
}

void NetSession::SetState(eSessionState sessionState)
{
	m_sessionState = sessionState;
}

std::string NetSession::GetSessionStateAsString()
{
	if (m_sessionState == SESSION_DISCONNECTED)
		return "DISCONNECTED";
	else if (m_sessionState == SESSION_CONNECTING)
		return "CONNECTING";
	else
		return "READY";
}

unsigned int NetSession::GetNumberOfValidConnections()
{
	unsigned int numConnections = 0;
	for (unsigned int connIndex = 0; connIndex < (unsigned int)m_connections.size(); connIndex++)
	{
		if (m_connections[connIndex] != nullptr)
			numConnections++;
	}
	return numConnections;
}
