#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetObjectConnectionState.hpp"
#include "Engine/Time/Time.hpp"

float NetObject::s_hostStartTimeRef = 0.f;
float NetObject::s_localStartTimeRef = 0.f;

NetObject::NetObject(NetObjectTypeDefinition *typeDefinition)
	: m_typeDefinition(typeDefinition)
	, m_netID(INVALID_NETWORK_ID)
	, m_localObject(nullptr)
	, m_currentSnapshot(nullptr)
	, m_lastReceivedSnapshot(nullptr)
	, m_localCurrentTimeRef(0.f)
	, m_hostCurrentTimeRef(0.f)
{
	for (int index = 0; index < 16; index++)
	{
		m_connectionStates.push_back(new NetObjectConnectionState());
	}
}

NetObject::~NetObject()
{

}

float NetObject::CalculateCurrentClientTime()
{
	m_localCurrentTimeRef = (m_hostCurrentTimeRef - s_hostStartTimeRef) + s_localStartTimeRef;
	return m_localCurrentTimeRef;
}

float NetObject::CalcClientDeltaSeconds()
{
	float currentTime = (float)GetCurrentTimeSeconds();
	return currentTime - m_localCurrentTimeRef;
}
