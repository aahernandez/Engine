#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include <math.h>

Vector3::Vector3()
	: x(0.f)
	, y(0.f)
	, z(0.f)
{
}


Vector3::Vector3(const Vector3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}


Vector3::Vector3(float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}

Vector3::Vector3(const Vector2& copy)
	: x(copy.x)
	, y(copy.y)
	, z(0)
{
}

void Vector3::GetXY(float& out_x, float& out_y, float& out_z) const
{
	out_x = x;
	out_y = y;
	out_z = z;
}

const float* Vector3::GetAsFloatArray() const
{
	return &x;
}

float* Vector3::GetAsFloatArray()
{
	return &x;
}

float Vector3::CalcLength() const
{
	return (float) sqrt( CalcLengthSquared() );
}

float Vector3::CalcLengthSquared() const
{
	return (float) ( (x*x) + (y*y) + (z*z) );
}

const float Vector3::CalcDistanceToVector(const Vector3& vectorB)
{
	float distanceX = vectorB.x - x;
	float distanceY = vectorB.y - y;
	float distanceZ = vectorB.z - z;
	return (float) sqrt( (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ) );
}

Vector3 Vector3::Absolute()
{
	Vector3 absoluteVector = *this;
	if (absoluteVector.x < 0.f)
		absoluteVector.x = -absoluteVector.x;
	if (absoluteVector.y < 0.f)
		absoluteVector.y = -absoluteVector.y;
	if (absoluteVector.z < 0.f)
		absoluteVector.z = -absoluteVector.z;
	return absoluteVector;
}

void Vector3::SetXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

float Vector3::Normalize()
{
	float length = CalcLength();
	x /= length;
	y /= length;
	z /= length;
	return length;
}

void Vector3::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Vector3::ScaleNonUniform(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void Vector3::InverseScaleNonUniform(const Vector3& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

bool Vector3::operator==(const Vector3& vectorToEqual) const
{
	return (x == vectorToEqual.x) 
		&& (y == vectorToEqual.y)
		&& (z == vectorToEqual.z);
}

bool Vector3::operator!=(const Vector3& vectorToNotEqual) const
{
	return (x != vectorToNotEqual.x) 
		|| (y != vectorToNotEqual.y)
		|| (z != vectorToNotEqual.z);
}

const Vector3 Vector3::operator+(const Vector3& vectorToAdd) const
{
	return Vector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

const Vector3 Vector3::operator-(const Vector3& vectorToSubtract) const
{
	return Vector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

const Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

const Vector3 Vector3::operator*(float scale) const
{
	return Vector3(x * scale, y * scale, z * scale);
}

const Vector3 Vector3::operator*(const Vector3& perAxisScaleFactors) const
{
	return Vector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

const Vector3 Vector3::operator/(float inverseScale) const
{
	return Vector3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vector3::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Vector3::operator*=(const Vector3& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void Vector3::operator+=(const Vector3& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

void Vector3::operator-=(const Vector3& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}

float CalcDistance(const Vector3& positionA, const Vector3& positionB)
{
	Vector3 distanceVector = positionB - positionA;
	distanceVector *= distanceVector;
	return (float) sqrt( distanceVector.x + distanceVector.y + distanceVector.z);
}

float CalcDistanceSquared(const Vector3& posA, const Vector3& posB)
{
	Vector3 distanceVector = posB - posA;
	distanceVector *= distanceVector;
	return distanceVector.x + distanceVector.y + distanceVector.z;
}

const Vector3 operator*(float scale, const Vector3 & vectorToScale)
{
	return vectorToScale * scale;
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

const Vector3 Vector3::ZERO(0.f, 0.f, 0.f);
const Vector3 Vector3::RIGHT(1.f, 0.f, 0.f);
const Vector3 Vector3::UP(0.f, 1.f, 0.f);
const Vector3 Vector3::FORWARD(0.f, 0.f, 1.f);