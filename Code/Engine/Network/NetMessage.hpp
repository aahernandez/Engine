#pragma once
#include "Engine/Streams/BinaryStream.hpp"
#include "Engine/Core/EngineCommon.hpp"

class NetConnection;

const int maxPayloadSize = 1 KB;

class NetMessage : public BinaryStream
{
public:
	uint8_t m_messageTypeIndex;
	NetConnection *m_senderConnection;
	byte_t m_payload[maxPayloadSize];
	unsigned int m_payloadBytesUsed;
	unsigned int m_bytesRead;

	NetMessage();
	NetMessage(uint8_t typeIndex);

	void ParsePayload();
	virtual unsigned int ReadBytes(void *outBuffer, unsigned int const count) override;
	virtual unsigned int WriteBytes(void const* buffer, unsigned int const size) override;
	std::string ReadString();
	void WriteString(std::string stringToWrite);
};