#pragma once

#include <vector>
#include <functional>

class NetConnection;
class NetMessageDefinition;
class NetMessage;
struct NetAddress_t;

constexpr uint8_t INVALID_CONNECTION_INDEX = 0xff;

enum eSessionState
{
	SESSION_DISCONNECTED,
	SESSION_CONNECTING,
	SESSION_CONNECTED,
	NUM_SESSION_STATES,
};

enum eCoreMessageID : uint8_t
{
	JOIN_RESPONSE,
	PING,
	PONG,
	RCS_COMMAND = 32,
	RCS_ECHO = 33,
	NUM_CORE_MESSAGE_IDS,
};

class NetSession
{
public:
	unsigned int m_maxConnectionCount;
	eSessionState m_sessionState;
	std::vector<NetConnection*> m_connections;
	NetConnection *m_myConnection;
	NetConnection *m_hostConnection;
	std::vector<NetMessageDefinition*> m_messageDefinitions;

	virtual bool Host(uint16_t port) = 0;
	virtual bool Join(NetAddress_t const &addr) = 0;
	virtual bool Join(std::string const &stringAddr) = 0;
	virtual void Leave() = 0;
	virtual void Update() = 0;

	virtual bool StartListening() = 0;
	virtual void StopListening() = 0;
	virtual bool IsListening() = 0;

	bool RegisterMessageDefinition(uint8_t messageID, NetMessageDefinition &definition);
	bool RegisterMessageFunction( uint8_t messageID, std::function<void(NetMessage*)> callback);
	NetMessageDefinition* GetMessageDefinition(uint8_t id) const;
		
	inline bool IsHost() const { return (m_myConnection == m_hostConnection) && (m_hostConnection != nullptr); }

	inline bool IsClient() const { return (m_myConnection != m_hostConnection) && (m_myConnection != nullptr); }

	inline bool IsRunning() const { return (m_myConnection != nullptr); }

	inline bool IsReady() const { return (m_sessionState == SESSION_CONNECTED); }

	uint8_t GetFreeConnectionIndex() const;
	void JoinConnection(uint8_t index, NetConnection *connection);
	void DestroyConnection(NetConnection *netConnection);
	NetConnection* GetConnection(uint8_t index);
	void SendMessageToOthers(NetMessage const &message);
	void SendMessageToIndex(int index, NetMessage const &message);
	void SendMessageToAll(NetMessage const &message);
	void SendMessageToHost(NetMessage const &message);
	void SendMessageToOthersButOne(NetMessage const &message, NetConnection *connection);
	void SetState(eSessionState sessionState);
	std::string GetSessionStateAsString();
	unsigned int GetNumberOfValidConnections();
};