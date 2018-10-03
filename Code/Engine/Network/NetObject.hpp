#pragma once
#include "Engine/Core/CommonInclude.hpp"
#include <limits>
#include <vector>

class NetObjectTypeDefinition;
class NetObjectConnectionState;

const unsigned short INVALID_NETWORK_ID = USHRT_MAX;

class NetObject
{
public:
	uint16_t m_netID;
	NetObjectTypeDefinition *m_typeDefinition;
	float m_hostCurrentTimeRef;
	float m_localCurrentTimeRef;
	void *m_localObject;
	void *m_currentSnapshot;
	void *m_lastReceivedSnapshot;
	std::vector<NetObjectConnectionState*> m_connectionStates;

	NetObject(NetObjectTypeDefinition *defn);
	~NetObject();

	float CalculateCurrentClientTime();
	float CalcClientDeltaSeconds();

	static float s_hostStartTimeRef;
	static float s_localStartTimeRef;
};