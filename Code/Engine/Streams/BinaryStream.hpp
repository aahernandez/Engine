#pragma once
#include <stdexcept>

typedef unsigned char byte_t;

enum eEndianness
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

static constexpr uint32_t ENDIAN_CHECK = 0x01020304;
bool constexpr IsBigEndian();
eEndianness constexpr GetHostOrder();
void CopyReversed(void *buffer, void const* src, unsigned int const size);

class BinaryStream
{
public:
	eEndianness stream_order;

	virtual unsigned int ReadBytes(void *out_buffer, unsigned int const count) = 0;
	virtual unsigned int WriteBytes(void const* buffer, unsigned int const size) = 0;

	unsigned int WriteEndianAwareBytes(void const *data, unsigned int data_size);
	unsigned int ReadEndianAwareBytes(void *data, unsigned int data_size);

	inline bool ShouldFlip() const { return stream_order != GetHostOrder(); }
};

