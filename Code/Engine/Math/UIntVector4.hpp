#pragma once

class UIntVector4
{
public:
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;

	UIntVector4();
	UIntVector4(const UIntVector4& copy);
	explicit UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW);
};