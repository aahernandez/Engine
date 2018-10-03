#pragma once
#include "Engine/Network/NetSession.hpp"

class TCPSocket;

class TCPSession : public NetSession
{
public:
	TCPSocket *m_listenSocket;

	TCPSession();
	virtual ~TCPSession();

	virtual bool Host(uint16_t port) override;
	virtual bool Join(NetAddress_t const &addr) override;
	virtual bool Join(std::string const &stringAddr) override;
	virtual void Leave() override;
	virtual void Update() override;

	virtual bool StartListening() override;
	virtual void StopListening() override;
	virtual bool IsListening() override;

	void SendJoinInfo(NetConnection *connection);
	void OnJoinResponse(NetMessage *message);
};