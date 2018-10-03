#include "Engine/Network/NetConnection.hpp"

NetConnection::~NetConnection()
{
	m_owner = nullptr;
}
