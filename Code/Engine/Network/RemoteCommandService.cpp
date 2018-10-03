#include "Engine/Network/RemoteCommandService.hpp"
#include "Engine/Network/TCPSession.hpp"
#include "Engine/Network/NetAddress.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/NetConnection.hpp"
#include "Engine/Network/NetMessageDefinition.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

RemoteCommandService *RemoteCommandService::s_instance = new RemoteCommandService();

RemoteCommandService::RemoteCommandService()
{
	m_session = new TCPSession();
	SetupMessageDefinitions();
	m_currentSender = nullptr;
	s_instance = this;
}

RemoteCommandService::~RemoteCommandService()
{
	s_instance = nullptr;
}

void RemoteCommandService::Update()
{
	if (m_session->IsRunning())
	{
		m_session->Update();
	}
	else
	{
		if (!m_session->Join(GetMyAddress(RCS_PORT)))
		{
			m_session->Host(RCS_PORT);
			m_session->StartListening();
		}
	}
}

void RemoteCommandService::Join(NetAddress_t const &addr)
{
	m_session->Leave();
	m_session->Join(addr);
}

void RemoteCommandService::Join(std::string addrString)
{
	std::vector<std::string> addressAndPort = ParseStringIntoPiecesByDelimiter(addrString, ":");
	int port = std::stoi(addressAndPort[1]);
	std::vector<NetAddress_t> addresses = GetAddressesFromHostName(addressAndPort[0].c_str(), (short)port, false);
	Join(addresses[0]);
}

void RemoteCommandService::SetupMessageDefinitions()
{
	NetMessageDefinition *pingMessageDef = new NetMessageDefinition();
	pingMessageDef->m_typeIndex = PING;
	pingMessageDef->m_handler = [](NetMessage *message)
	{
		DebuggerPrintf("PING");
		NetMessage *pong = new NetMessage(PONG);
		message->m_senderConnection->Send(pong);
	};

	NetMessageDefinition *pongMessageDef = new NetMessageDefinition();
	pongMessageDef->m_typeIndex = PONG;
	pongMessageDef->m_handler = [](NetMessage *message)
	{
		message;
		DebuggerPrintf("PONG");
	};

	m_session->RegisterMessageDefinition(PING, *pingMessageDef);
	m_session->RegisterMessageDefinition(PONG, *pongMessageDef);
	m_session->RegisterMessageFunction(RCS_COMMAND, [&](NetMessage* message){OnCommand(message);} );
	m_session->RegisterMessageFunction(RCS_ECHO, [&](NetMessage* message){OnMessage(message);} );
}

void SendMessage(void *userArg, void *eventArg)
{
	NetMessage *message = new NetMessage(RCS_ECHO);
	const char *userAddress = (const char*)userArg;
	ColoredText *consoleMessage = (ColoredText*)(eventArg);
	std::string stringToWrite = "[";
	stringToWrite += userAddress;
	stringToWrite += "] ";
	stringToWrite += consoleMessage->m_text;
	message->WriteString(stringToWrite);
	if (RemoteCommandService::GetInstance()->m_currentSender != nullptr)
		RemoteCommandService::GetInstance()->m_currentSender->Send(message);
}

void RemoteCommandService::OnCommand(NetMessage *message)
{
	std::string commandAndArgs = message->ReadString();
	m_currentSender = message->m_senderConnection;
	std::string address = NetAddressToString(m_currentSender->m_address);
// 	g_theDevConsole->ConsolePrintf("[%s] %s", address.c_str(), commandAndArgs.c_str());

// 	g_theDevConsole->SubscribeToEvent((void*)address.c_str(), SendMessage);
	g_theDevConsole->RunCommand(commandAndArgs);
// 	g_theDevConsole->UnsubscribeToEvent((void*)commandAndArgs.c_str(), SendMessage);
// 	m_currentSender = nullptr;
}

void RemoteCommandService::OnMessage(NetMessage *message)
{
	std::string response = message->ReadString();
	std::string address = NetAddressToString(message->m_senderConnection->m_address);
	std::string stringToPrint = Stringf("[%s] %s", address.c_str(), response.c_str());
	g_theDevConsole->ConsolePrintf(stringToPrint.c_str());
}

void RemoteCommandService::SendCommandToOthers(std::string commandAndArgs)
{
	NetMessage message(RCS_COMMAND);
	message.WriteString(commandAndArgs);
	m_session->SendMessageToOthers(message);
}

void RemoteCommandService::SendCommandToSession(int index, std::string commandAndArgs)
{
	NetMessage message(RCS_COMMAND);
	message.WriteString(commandAndArgs);
	m_session->SendMessageToIndex(index, message);
}

void RemoteCommandService::SendCommandToAll(std::string commandAndArgs)
{
	NetMessage message(RCS_COMMAND);
	message.WriteString(commandAndArgs);
	m_session->SendMessageToAll(message);
}

RemoteCommandService* RemoteCommandService::GetInstance()
{
	return s_instance;
}
