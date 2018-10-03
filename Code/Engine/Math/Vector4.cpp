#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include <math.h>

Vector4::Vector4()
	: x(0.f)
	, y(0.f)
	, z(0.f)
	, w(0.f)
{
}


Vector4::Vector4(const Vector4& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}


Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}

Vector4::Vector4(const Vector3& copy, float initialW)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(initialW)
{
}

void Vector4::GetXY(float& out_x, float& out_y, float& out_z, float& out_w) const
{
	out_x = x;
	out_y = y;
	out_z = z;
	out_w = w;
}

const float* Vector4::GetAsFloatArray() const
{
	return &x;
}

float* Vector4::GetAsFloatArray()
{
	return &x;
}

float Vector4::CalcLength() const
{
	return (float) sqrt( CalcLengthSquared() );
}

float Vector4::CalcLengthSquared() const
{
	return (float) ( (x*x) + (y*y) + (z*z) + (w*w) );
}

const float Vector4::CalcDistanceToVector(const Vector4& vectorB)
{
	float distanceX = vectorB.x - x;
	float distanceY = vectorB.y - y;
	float distanceZ = vectorB.z - z;
	float distanceW = vectorB.w - w;
	return (float) sqrt( (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ) + (distanceW * distanceW) );
}

void Vector4::SetXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

void Vector4::SetXYZW(float newX, float newY, float newZ, float newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

float Vector4::Normalize3D()
{
	float length = CalcLength();
	x /= length;
	y /= length;
	z /= length;
	return length;
}

float Vector4::Normalize4D()
{
	float length = CalcLength();
	x /= length;
	y /= length;
	z /= length;
	w /= length;
	return length;
}

void Vector4::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::ScaleNonUniform(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void Vector4::InverseScaleNonUniform(const Vector4& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

bool Vector4::operator==(const Vector4& vectorToEqual) const
{
	return (x == vectorToEqual.x)
		&& (y == vectorToEqual.y)
		&& (z == vectorToEqual.z)
		&& (w == vectorToEqual.w);
}

bool Vector4::operator!=(const Vector4& vectorToNotEqual) const
{
	return (x != vectorToNotEqual.x)
		|| (y != vectorToNotEqual.y)
		|| (z != vectorToNotEqual.z)
		|| (w != vectorToNotEqual.w);
}

const Vector4 Vector4::operator+(const Vector4& vectorToAdd) const
{
	return Vector4(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z, w + vectorToAdd.w);
}

const Vector4 Vector4::operator-(const Vector4& vectorToSubtract) const
{
	return Vector4(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z, w-vectorToSubtract.w);
}

Vector4 Vector4::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

void Vector4::operator=(const RGBA color)
{
	color.GetAsFloats(x, y, z, w);
}

const Vector4 Vector4::operator*(float scale) const
{
	return Vector4(x * scale, y * scale, z * scale, w * scale);
}

const Vector4 Vector4::operator*(const Vector4& perAxisScaleFactors) const
{
	return Vector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const Vector4 Vector4::operator/(float inverseScale) const
{
	return Vector4(x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale);
}

void Vector4::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::operator*=(const Vector4& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void Vector4::operator+=(const Vector4& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
	w += vectorToAdd.w;
}

void Vector4::operator-=(const Vector4& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	w -= vectorToSubtract.w;
}

float CalcDistance(const Vector4& positionA, const Vector4& positionB)
{
	Vector4 distanceVector = positionB - positionA;
	distanceVector *= distanceVector;
	return (float) sqrt( distanceVector.x + distanceVector.y + distanceVector.z + distanceVector.w);
}

float CalcDistanceSquared(const Vector4& posA, const Vector4& posB)
{
	Vector4 distanceVector = posB - posA;
	distanceVector *= distanceVector;
	return distanceVector.x + distanceVector.y + distanceVector.z + distanceVector.w;
}

const Vector4 operator*(float scale, const Vector4 & vectorToScale)
{
	return vectorToScale * scale;
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}