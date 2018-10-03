//Some code based off of code by Squirrel Eiserloh
#include "Engine/Math/IntVector3.hpp"
#include <math.h>

IntVector3::IntVector3()
	: x(0)
	, y(0)
	, z(0)
{
}


IntVector3::IntVector3(const IntVector3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{

}

IntVector3::IntVector3(const Vector3& copy)
{
	x = (int)floor(copy.x);
	y = (int)floor(copy.y);
	z = (int)floor(copy.z);
}

void IntVector3::GetXYZ(int& out_x, int& out_y, int& out_z) const
{
	out_x = x;
	out_y = y;
	out_z = z;
}

const int* IntVector3::GetAsIntArray() const
{
	return &x;
}

int* IntVector3::GetAsIntArray()
{
	return &x;
}

int IntVector3::CalcProduct() const
{
	return x * y * z;
}

int IntVector3::CalcLength() const
{
	return (int)sqrt((x*x) + (y*y) + (z*z));
}

int IntVector3::CalcLengthSquared() const
{
	return ((x*x) + (y*y) + (z*z));
}

const int IntVector3::CalcDistanceToIntVector(const IntVector3& vectorB)
{
	int distanceX = vectorB.x - x;
	int distanceY = vectorB.y - y;
	int distanceZ = vectorB.z - z;
	return (int)sqrt((distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ));
}


void IntVector3::SetXYZ(int newX, int newY, int newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

void IntVector3::ScaleUniform(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void IntVector3::ScaleNonUniform(const IntVector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void IntVector3::InverseScaleNonUniform(const IntVector3& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

bool IntVector3::operator==(const IntVector3& vectorToEqual) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y) && z == vectorToEqual.z;
}

bool IntVector3::operator!=(const IntVector3& vectorToNotEqual) const
{
	return (x != vectorToNotEqual.x) || (y != vectorToNotEqual.y) || (y != vectorToNotEqual.z);
}

const IntVector3 IntVector3::operator+(const IntVector3& vectorToAdd) const
{
	return IntVector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

const IntVector3 IntVector3::operator-(const IntVector3& vectorToSubtract) const
{
	return IntVector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

const IntVector3 IntVector3::operator*(const IntVector3& perAxisScaleFactors) const
{
	return IntVector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

const IntVector3 IntVector3::operator*(int scale) const
{
	return IntVector3(x * scale, y * scale, z * scale);
}

const IntVector3 IntVector3::operator/(int inverseScale) const
{
	return IntVector3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void IntVector3::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void IntVector3::operator*=(const IntVector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void IntVector3::operator+=(const IntVector3& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

void IntVector3::operator-=(const IntVector3& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}

int CalcDistance(const IntVector3& positionA, const IntVector3& positionB)
{
	IntVector3 distanceVector = positionB - positionA;
	distanceVector *= distanceVector;
	return (int)sqrt(distanceVector.x + distanceVector.y + distanceVector.z);
}

int CalcDistanceSquared(const IntVector3& posA, const IntVector3& posB)
{
	IntVector3 distanceVector = posB - posA;
	distanceVector *= distanceVector;
	return distanceVector.x + distanceVector.y + distanceVector.z;
}

int DotProduct(const IntVector3& a, const IntVector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}