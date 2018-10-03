#pragma once
#include <string>

class TCPSession;
class NetConnection;
class NetMessage;

struct NetAddress_t;

const unsigned short RCS_PORT = 44514;

class RemoteCommandService
{
private:
	static RemoteCommandService *s_instance;

	RemoteCommandService();
	~RemoteCommandService();


public:
	TCPSession *m_session;
	NetConnection *m_currentSender;

	void Update();
	void Join(NetAddress_t const &addr);
	void Join(std::string addrString);
	void SetupMessageDefinitions();
	void OnCommand(NetMessage *message);
	void OnMessage(NetMessage *message);
	void SendCommandToOthers(std::string commandAndArgs);
	void SendCommandToSession(int index, std::string commandAndArgs);
	void SendCommandToAll(std::string commandAndArgs);

	static RemoteCommandService* GetInstance();
};