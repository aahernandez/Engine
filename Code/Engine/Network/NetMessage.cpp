#include "Engine/Network/NetMessage.hpp"

NetMessage::NetMessage()
	: m_payloadBytesUsed(0)
	, m_bytesRead(0)
{
}

NetMessage::NetMessage(uint8_t typeIndex)
	: m_payloadBytesUsed(0)
	, m_bytesRead(0)
{
	m_messageTypeIndex = typeIndex;
}

void NetMessage::ParsePayload()
{
	m_messageTypeIndex = m_payload[2];
	m_payloadBytesUsed = (m_payload[1] << 8) | m_payload[0];
}

unsigned int NetMessage::ReadBytes(void *outBuffer, unsigned int const count)
{
	unsigned int numBytesRead = 0;
	for (unsigned int index = m_bytesRead; index < count + m_bytesRead; index++)
	{
		if (m_payload != NULL)
		{
			((byte_t*)outBuffer)[index - m_bytesRead] = m_payload[index];
			numBytesRead++;
		}
		else
		{
			return numBytesRead;
		}
	}
	m_bytesRead += numBytesRead;
	return numBytesRead;
}

unsigned int NetMessage::WriteBytes(void const* buffer, unsigned int const size)
{
	memcpy(&m_payload[m_payloadBytesUsed], buffer, size);
	m_payloadBytesUsed += size;
	return m_payloadBytesUsed;
}

std::string NetMessage::ReadString()
{
	unsigned int sizeOfString = (m_payload[m_bytesRead + 1] << 8) | m_payload[m_bytesRead];
	m_bytesRead += 2;
	std::string returnString;
	returnString.resize(sizeOfString);
	for (unsigned int index = 0; index < sizeOfString; index++)
	{
		returnString[index] = m_payload[index + m_bytesRead];
	}
	m_bytesRead += sizeOfString;

	return returnString;
}

void NetMessage::WriteString(std::string stringToWrite)
{
	unsigned int sizeOfString = stringToWrite.size();
	m_payload[m_payloadBytesUsed] = sizeOfString & 0xff;
	m_payload[m_payloadBytesUsed + 1] = (sizeOfString >> (8)) & 0xff;
	m_payloadBytesUsed += 2;

	for (unsigned int index = 0; index < sizeOfString; index++)
	{
		m_payload[m_payloadBytesUsed + index] = ((byte_t)stringToWrite[index]);
	}
	m_payloadBytesUsed += sizeOfString;
}

