#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <functional>

class NetMessage;

typedef std::function<void(NetMessage* message)> MessageDefFunc;

class NetMessageDefinition
{
public:
	uint8_t m_typeIndex;
	std::function<void(NetMessage*)> m_handler;
};