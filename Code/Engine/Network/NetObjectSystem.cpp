#include "Engine/Time/Time.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/NetSession.hpp"
#include "Engine/Network/TCPConnection.hpp"
#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetObjectTypeDefinition.hpp"

NetObjectSystem::NetObjectSystem()
{
	m_netObjects.resize(USHRT_MAX);
	for (unsigned short index = 0; index < m_netObjects.size(); index++)
	{
		m_netObjects[index] = nullptr;
	}
	
	m_typeDefinitions.resize(UCHAR_MAX);
	for (unsigned short index = 0; index < m_typeDefinitions.size(); index++)
	{
		m_typeDefinitions[index] = nullptr;
	}

	m_netSession = nullptr;

	m_timer = new Timer(30.f);
}

NetObjectSystem::~NetObjectSystem()
{

}

unsigned short NetObjectSystem::GetUnusedID()
{
	for (unsigned short index = 0; index < m_netObjects.size(); index++)
	{
		if (m_netObjects[index] == nullptr)
			return index;
	}
	return INVALID_NETWORK_ID;
}

NetObject* NetObjectSystem::Replicate(void *objectPtr, unsigned char typeID)
{
	NetObjectTypeDefinition *typeDefinition = FindDefinition(typeID);
	if (typeDefinition == nullptr)
	{
		return nullptr;
	}

	NetObject *netObject = new NetObject(typeDefinition);
	netObject->m_localObject = objectPtr;
	netObject->m_netID = GetUnusedID();
	ASSERT_OR_DIE(netObject->m_netID != INVALID_NETWORK_ID, "Net ID is invalid.");

	Register(netObject);

	NetMessage createMessage(NETOBJECT_CREATE_OBJECT);
	createMessage.WriteBytes(&typeID, sizeof(uint8_t));
	createMessage.WriteBytes(&netObject->m_netID, sizeof(unsigned short));
	typeDefinition->append_create_info(&createMessage, objectPtr);

	NetSession *session = GetNetObjectSession();
	session->SendMessageToOthers(createMessage);

	return netObject;
}

NetObjectTypeDefinition* NetObjectSystem::FindDefinition(unsigned char typeID)
{
	return m_typeDefinitions[typeID];
}

void NetObjectSystem::Register(NetObject *netObject)
{
	m_netObjects[netObject->m_netID] = netObject;
}

void NetObjectSystem::Unregister(NetObject *netObject)
{
	uint16_t netID = netObject->m_netID;
	delete m_netObjects[netID];
	m_netObjects[netID] = nullptr;
}

void NetObjectSystem::RegisterNetObjectType(uint8_t typeDefinitionID, NetObjectTypeDefinition* typeDefinition)
{
	typeDefinition->m_typeID = typeDefinitionID;
	m_typeDefinitions[typeDefinitionID] = typeDefinition;
}

void NetObjectSystem::RegisterNetSession(NetSession *newNetSession)
{
	m_netSession = newNetSession;
	m_netSession->RegisterMessageFunction(NETOBJECT_CREATE_OBJECT, [&](NetMessage* message) {OnReceiveNetObjectCreate(message); });
	m_netSession->RegisterMessageFunction(NETOBJECT_DESTROY_OBJECT, [&](NetMessage* message) {OnNetObjectReceiveDestroy(message); });
	m_netSession->RegisterMessageFunction(NETOBJECT_UPDATE, [&](NetMessage* message) {OnNetObjectUpdateRecieved(message); });
	m_netSession->RegisterMessageFunction(NETOBJECTSYSTEM_CREATE_HOST_TIME, [&](NetMessage* message) {OnNetObjectSystemHostTimeReceived(message); });
}

NetSession* NetObjectSystem::GetNetObjectSession()
{
	return m_netSession;
}

void NetObjectSystem::OnReceiveNetObjectCreate(NetMessage *message)
{
	uint8_t typeID;
	message->ReadBytes(&typeID, sizeof(uint8_t));
	uint16_t netID;
	message->ReadBytes(&netID, sizeof(uint16_t));

	NetObjectTypeDefinition *typeDefinition = FindDefinition(typeID);
	ASSERT_OR_DIE(typeDefinition != nullptr, "Net Object definition is nullptr.");

	NetObject *netObject = new NetObject(typeDefinition);
	netObject->m_netID = netID;

	void *localObject = typeDefinition->process_create_info(message, netObject);
	ASSERT_OR_DIE(localObject != nullptr, "local_object is nullptr.");

	netObject->m_localObject = localObject;
	Register(netObject);
}

NetObject* NetObjectSystem::Find(uint16_t netID)
{
	if (netID == INVALID_NETWORK_ID)
		return nullptr;
	return m_netObjects.at(netID);
}

void NetObjectSystem::NetObjectStopRelication(uint16_t netID)
{
	NetObject *netObject = Find(netID);
	if (netObject == nullptr)
	{
		return;
	}

	NetMessage message(NETOBJECT_DESTROY_OBJECT);
	message.WriteBytes(&netObject->m_netID, sizeof(uint16_t));

	netObject->m_typeDefinition->append_destroy_info(&message, netObject->m_localObject);

	NetSession *session = GetNetObjectSession();
	session->SendMessageToOthers(message);
	
	Unregister(netObject);
}

void NetObjectSystem::OnNetObjectReceiveDestroy(NetMessage *message)
{
	uint16_t netID;
	message->ReadBytes(&netID, sizeof(uint16_t));
	NetObject *netObject = Find(netID);

	if (netObject == nullptr) 
	{
		return;
	}

	netObject->m_typeDefinition->process_destroy_info(message, netObject->m_localObject);
	
	Unregister(netObject);
}

void NetObjectSystem::OnNetObjectUpdateRecieved(NetMessage *updateMessage)
{
	float hostTime = 0.f;
	updateMessage->ReadBytes(&hostTime, sizeof(float));
	unsigned int numUpdates = 0;
	updateMessage->ReadBytes(&numUpdates, sizeof(unsigned int));

	for (unsigned int updateCount = 0; updateCount < numUpdates; updateCount++)
	{
		uint16_t net_id = INVALID_NETWORK_ID;
		updateMessage->ReadBytes(&net_id, sizeof(uint16_t));

		NetObject *netObject = Find(net_id);
		if (netObject == nullptr)
		{
			return;
		}

		netObject->m_hostCurrentTimeRef = hostTime;
		netObject->m_typeDefinition->process_snapshot(netObject->m_lastReceivedSnapshot, updateMessage);
	}
}

void NetObjectSystem::OnNetObjectSystemHostTimeReceived(NetMessage *updateMessage)
{
	updateMessage->ReadBytes(&NetObject::s_hostStartTimeRef, sizeof(float));
	NetObject::s_localStartTimeRef = (float)GetCurrentTimeSeconds();
}

void NetObjectSystem::CreateHostTimeMessageAndSend(NetConnection *connection)
{
	NetMessage hostTimeMessage(NETOBJECTSYSTEM_CREATE_HOST_TIME);
	float currentTime = (float)GetCurrentTimeSeconds();
	hostTimeMessage.WriteBytes(&currentTime, sizeof(float));
	connection->Send(&hostTimeMessage);
}

void NetObjectSystem::ReplicateToNewConnection(NetConnection *connection, NetObject *netObject)
{
// 	NetObjectTypeDefinition *typeDefinition = FindDefinition(typeID);
// 	if (typeDefinition == nullptr)
// 	{
// 		return nullptr;
// 	}

// 	NetObject *netObject = new NetObject(typeDefinition);
// 	netObject->m_localObject = objectPtr;
// 	netObject->m_netID = GetUnusedID();
// 	ASSERT_OR_DIE(netObject->m_netID != INVALID_NETWORK_ID, "Net ID is invalid.");

// 	Register(netObject);

	NetMessage createMessage(NETOBJECT_CREATE_OBJECT);
	createMessage.WriteBytes(&netObject->m_typeDefinition->m_typeID, sizeof(uint8_t));
	createMessage.WriteBytes(&netObject->m_netID, sizeof(unsigned short));
	netObject->m_typeDefinition->append_create_info(&createMessage, netObject->m_localObject);

	connection->Send(&createMessage);
// 	NetSession *session = GetNetObjectSession();
// 	session->SendMessageToOthers(createMessage);

// 	return netObject;
}
