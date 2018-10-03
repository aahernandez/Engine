//Some code based off of code by Squirrel Eiserloh
#include "Engine/Math/IntVector2.hpp"
#include <math.h>

IntVector2::IntVector2()
	: x(0)
	, y(0)
{
}


IntVector2::IntVector2(const IntVector2& copy)
	: x(copy.x)
	, y(copy.y)
{
}

IntVector2::IntVector2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{

}

IntVector2::IntVector2(const Vector2& copy)
{
	x = (int) floor(copy.x);
	y = (int) floor(copy.y);
}

IntVector2::IntVector2(float initialX, float initialY)
{
	x = (int) floor(initialX);
	y = (int) floor(initialY);
}

void IntVector2::GetXY(int& out_x, int& out_y) const
{
	out_x = x;
	out_y = y;
}

const int* IntVector2::GetAsIntArray() const
{
	return &x;
}

int* IntVector2::GetAsIntArray()
{
	return &x;
}

int IntVector2::CalcProduct() const
{
	return x * y;
}

int IntVector2::CalcLength() const
{
	return (int)sqrt((x*x) + (y*y));
}

int IntVector2::CalcLengthSquared() const
{
	return ((x*x) + (y*y));
}

const int IntVector2::CalcDistanceToIntVector(const IntVector2& vectorB)
{
	int distanceX = vectorB.x - x;
	int distanceY = vectorB.y - y;
	return (int) sqrt((distanceX * distanceX) + (distanceY * distanceY));
}


void IntVector2::SetXY(int newX, int newY)
{
	x = newX;
	y = newY;
}

void IntVector2::ScaleUniform(int scale)
{
	x *= scale;
	y *= scale;
}

void IntVector2::ScaleNonUniform(const IntVector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void IntVector2::InverseScaleNonUniform(const IntVector2& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

bool IntVector2::operator==(const IntVector2& vectorToEqual) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y);
}

bool IntVector2::operator!=(const IntVector2& vectorToNotEqual) const
{
	return (x != vectorToNotEqual.x) || (y != vectorToNotEqual.y);
}

const IntVector2 IntVector2::operator+(const IntVector2& vectorToAdd) const
{
	return IntVector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

const IntVector2 IntVector2::operator-(const IntVector2& vectorToSubtract) const
{
	return IntVector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

const IntVector2 IntVector2::operator*(const IntVector2& perAxisScaleFactors) const
{
	return IntVector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

const IntVector2 IntVector2::operator*(int scale) const
{
	return IntVector2(x * scale, y * scale);
}

const IntVector2 IntVector2::operator/(int inverseScale) const
{
	return IntVector2(x / inverseScale, y / inverseScale);
}

void IntVector2::operator*=(int scale)
{
	x *= scale;
	y *= scale;
}

void IntVector2::operator*=(const IntVector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void IntVector2::operator+=(const IntVector2& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

void IntVector2::operator-=(const IntVector2& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

bool operator < ( const IntVector2& lhs, const IntVector2& rhs )
{
	if (lhs.y < rhs.y)
		return true;
	else if (lhs.y > rhs.y)
		return false;
	else
		return lhs.x < rhs.x;
}

int CalcDistance(const IntVector2& positionA, const IntVector2& positionB)
{
	IntVector2 distanceVector = positionB - positionA;
	distanceVector *= distanceVector;
	return (int) sqrt(distanceVector.x + distanceVector.y);
}

int CalcDistanceSquared(const IntVector2& posA, const IntVector2& posB)
{
	IntVector2 distanceVector = posB - posA;
	distanceVector *= distanceVector;
	return distanceVector.x + distanceVector.y;
}

int DotProduct(const IntVector2& a, const IntVector2& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

const IntVector2 IntVector2::ZERO = IntVector2(0.f, 0.f);