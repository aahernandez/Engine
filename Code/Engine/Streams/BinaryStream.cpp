#include "Engine/Streams/BinaryStream.hpp"

unsigned int BinaryStream::WriteEndianAwareBytes(void const *data, unsigned int data_size)
{
	if (ShouldFlip())
	{
		byte_t *temp = (byte_t*)_alloca(data_size);
		CopyReversed(temp, data, data_size);
		return WriteBytes(temp, data_size);
	}
	else
	{
		return WriteBytes(data, data_size);
	}
}

unsigned int BinaryStream::ReadEndianAwareBytes(void *data, unsigned int data_size)
{
	unsigned int numBytes = ReadBytes(data, data_size);

	if (ShouldFlip())
	{
		byte_t *temp = (byte_t*)_alloca(data_size);
		CopyReversed(temp, data, data_size);
		data = temp;
		return numBytes;
	}
	else
	{
		return numBytes;
	}
}

void CopyReversed(void *buffer, void const* src, unsigned int const size)
{
	unsigned int lastIndex = size - 1;
	for (unsigned int index = 0; index < size; index++)
	{
		((byte_t*)buffer)[index] = ((byte_t*)src)[lastIndex - index];
	}
}

bool constexpr IsBigEndian()
{
	return ((byte_t*)(&ENDIAN_CHECK))[0] == 0x01;
}

eEndianness constexpr GetHostOrder()
{
	return IsBigEndian() ? BIG_ENDIAN : LITTLE_ENDIAN;
}