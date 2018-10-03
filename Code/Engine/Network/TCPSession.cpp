#include "Engine/Network/TCPSession.hpp"
#include "Engine/Network/LoopBackConnection.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/NetMessageDefinition.hpp"
#include "Engine/Network/TCPConnection.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

TCPSession::TCPSession()
{
	m_myConnection = nullptr;
	m_hostConnection = nullptr;
	m_listenSocket = nullptr;
	m_maxConnectionCount = 16;
	RegisterMessageFunction(JOIN_RESPONSE, [&](NetMessage* message){OnJoinResponse(message);} );
}

TCPSession::~TCPSession()
{

}

bool TCPSession::Host(uint16_t port)
{
	ASSERT_OR_DIE(!IsRunning(), "Host is running.");

	m_myConnection = new LoopBackConnection();
	m_myConnection->m_address = GetMyAddress(port);
	JoinConnection(0, m_myConnection);
	m_hostConnection = m_myConnection;

	SetState(SESSION_CONNECTED);
	return true;
}

bool TCPSession::Join(const NetAddress_t &address)
{
	TCPConnection *host = new TCPConnection();
	host->m_address = address;

	JoinConnection(0, host);
	host->m_socket = new TCPSocket();
	host->m_socket->SetBlocking(false);
	m_hostConnection = host;

	if (!host->Connect())
	{
		Leave();
		return false;
	}

	m_myConnection = new LoopBackConnection();
	m_myConnection->m_address = GetMyAddress();

	SetState(SESSION_CONNECTING);
	return true;
}

bool TCPSession::Join(std::string const &stringAddr)
{
	std::vector<std::string> addressAndPort = ParseStringIntoPiecesByDelimiter(stringAddr, ":");
	int port = std::stoi(addressAndPort[1]);
	std::vector<NetAddress_t> addresses = GetAddressesFromHostName(addressAndPort[0].c_str(), (short)port, false);
	return Join(addresses[0]);
}

void TCPSession::Leave()
{
	DestroyConnection(m_myConnection);
	DestroyConnection(m_hostConnection);

	for (unsigned int index = 0; index < m_connections.size(); index++)
	{
		DestroyConnection(m_connections[index]);
	}

	StopListening();
	SetState(SESSION_DISCONNECTED);
}

void TCPSession::Update()
{
	if (IsListening())
	{
		TCPSocket *socket = m_listenSocket->Accept();
		if (socket != nullptr)
		{
			TCPConnection *newConnection = new TCPConnection();
			newConnection->m_socket = socket;
			newConnection->m_socket->SetBlocking(false);
			newConnection->m_address = newConnection->m_socket->m_address;

			uint8_t connectionIndex = GetFreeConnectionIndex();
			if (connectionIndex == INVALID_CONNECTION_INDEX)
			{
				delete newConnection;
			}
			else
			{
				JoinConnection(connectionIndex, newConnection);
				SendJoinInfo(newConnection);
			}
		}
	}

	std::vector<NetConnection*>::iterator connectionIter;
	for (connectionIter = m_connections.begin(); connectionIter != m_connections.end(); ++connectionIter)
	{
		NetConnection *connection = *connectionIter;
		if ((connection != nullptr) && (connection != m_myConnection)) {
			TCPConnection *tcpConnection = (TCPConnection*)connection;
			if (tcpConnection->IsDisconnected()) 
			{
				DestroyConnection(tcpConnection);

			}
		}
	}

	for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); connectionIndex++)
	{
		NetMessage *message = nullptr;
		
		NetConnection *connection = m_connections[connectionIndex];
		if (connection == nullptr)
			continue;

		((TCPConnection*)connection)->m_socket->SetBlocking(false);
		while (connection->Receive(&message) && message != nullptr)
		{
			message->m_senderConnection = connection;
			if (m_messageDefinitions.size() > message->m_messageTypeIndex && m_messageDefinitions.at(message->m_messageTypeIndex) != nullptr)
				m_messageDefinitions[message->m_messageTypeIndex]->m_handler(message);
		}
	}

	if (m_hostConnection == nullptr)
	{
		Leave();
	}
}

bool TCPSession::StartListening()
{
	if (!IsHost())
		return false;

	if (IsListening())
		return true;

	m_listenSocket = new TCPSocket();
	if (m_listenSocket->Listen(m_myConnection->m_address.port))
	{
		m_listenSocket->SetBlocking(false);
		return true;
	}
	else
	{
		StopListening();
		return false;
	}
}

void TCPSession::StopListening()
{
	if (IsListening())
	{
		delete m_listenSocket;
		m_listenSocket = nullptr;
 	}
}

void TCPSession::SendJoinInfo(NetConnection *connection)
{
	NetMessage *message = new NetMessage(JOIN_RESPONSE);
	message->WriteBytes(&connection->m_connectionIndex, sizeof(uint8_t));
	connection->Send(message);
}

void TCPSession::OnJoinResponse(NetMessage *message)
{
	uint8_t myConnectionIndex;
	message->ReadBytes(&myConnectionIndex, sizeof(uint8_t));
	JoinConnection(myConnectionIndex, m_myConnection);
	((TCPConnection*)m_myConnection)->m_socket->SetBlocking(false);
	SetState(SESSION_CONNECTED);
}

bool TCPSession::IsListening()
{
	return m_listenSocket != nullptr;
}
