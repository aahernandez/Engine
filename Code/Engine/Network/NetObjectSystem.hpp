#pragma once
#include <vector>

class Timer;
class NetObject;
class NetSession;
class NetMessage;
class NetConnection;
class NetObjectTypeDefinition;

enum eNetObjectMessageID : uint8_t
{
	NETOBJECT_CREATE_OBJECT = 9,
	NETOBJECT_DESTROY_OBJECT = 10,
	NETOBJECT_UPDATE = 11,
	NETOBJECTSYSTEM_CREATE_HOST_TIME = 12,
	NUM_NETOBJECT_MESSAGE_IDS,
};

class NetObjectSystem
{
public:
	Timer *m_timer;
	NetSession *m_netSession;
	std::vector<NetObject*> m_netObjects;
	std::vector<NetObjectTypeDefinition*> m_typeDefinitions;
	
	NetObjectSystem();
	~NetObjectSystem();

	unsigned short GetUnusedID();
	NetObject* Replicate(void *object_ptr, unsigned char type_id);
	NetObjectTypeDefinition* FindDefinition(unsigned char type_id);
	void Register(NetObject *netObject);
	void Unregister(NetObject *netObject);
	void RegisterNetObjectType(uint8_t typeDefinitionID, NetObjectTypeDefinition* typeDefinition);
	void RegisterNetSession(NetSession *newNetSession);
	NetSession* GetNetObjectSession();
	void OnReceiveNetObjectCreate(NetMessage *msg);
	NetObject* Find(uint16_t netID);
	void NetObjectStopRelication(uint16_t netID);
	void OnNetObjectReceiveDestroy(NetMessage *msg);
	void OnNetObjectUpdateRecieved(NetMessage *update_msg);
	void OnNetObjectSystemHostTimeReceived(NetMessage *updateMessage);
	void CreateHostTimeMessageAndSend(NetConnection *connection);
	void ReplicateToNewConnection(NetConnection *connection, NetObject *netObject);
};