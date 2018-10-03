#pragma once

class NetObjectConnectionState
{
public:
	void *m_lastSentSnapshot;

	NetObjectConnectionState();
	~NetObjectConnectionState();
};