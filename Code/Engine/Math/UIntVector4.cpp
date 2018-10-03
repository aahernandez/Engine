#include "Engine/Math/UIntVector4.hpp"

UIntVector4::UIntVector4()
	: x(0)
	, y(0)
	, z(0)
	, w(0)
{
}

UIntVector4::UIntVector4(const UIntVector4& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}

UIntVector4::UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}

