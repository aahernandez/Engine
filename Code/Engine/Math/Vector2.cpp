//Some code based off of code by Squirrel Eiserloh
#include "Engine/Math/Vector2.hpp"

Vector2::Vector2()
	: x(0.0f)
	, y(0.0f)
{
}


Vector2::Vector2(const Vector2& copy) 
	: x(copy.x)
	, y(copy.y)
{
}


Vector2::Vector2(float initialX, float initialY) 
	: x(initialX)
	, y(initialY)
{
}

Vector2::Vector2(const IntVector2& copy)
	: x((float)copy.x)
	, y((float)copy.y)
{
}

Vector2::Vector2(int initialX, int initialY)
	: x((float)initialX)
	, y((float)initialY)
{
}

void Vector2::GetXY(float& out_x, float& out_y) const
{
	out_x = x;
	out_y = y;
}

const float* Vector2::GetAsFloatArray() const
{
	return &x;
}

float* Vector2::GetAsFloatArray()
{
	return &x;
}

float Vector2::CalcLength() const
{
	return (float) sqrt( (x*x) + (y*y) );
}

float Vector2::CalcLengthSquared() const
{
	return (float) ( (x*x) + (y*y) );
}

float Vector2::CalcHeadingDegrees() const
{
	float radians = CalcHeadingRadians();
	return ConvertRadiansToDegrees(radians);
}

float Vector2::CalcHeadingRadians() const
{
	return (float) atan2(y,x);
}

const float Vector2::CalcDistanceToVector(const Vector2& vectorB)
{
	float distanceX = vectorB.x - x;
	float distanceY = vectorB.y - y;
	return (float)sqrt((distanceX * distanceX) + (distanceY * distanceY));
}

void Vector2::SetXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

void Vector2::Rotate90Degrees()
{
	float newX = -y;
	y = x;
	x = newX;
}

void Vector2::RotateNegative90Degrees()
{
	float newX = y;
	y = -x;
	x = newX;
}

void Vector2::RotateDegrees(float degrees)
{
	float length = CalcLength();
	float heading = CalcHeadingDegrees();
	heading += degrees;
	x = length * CosDegrees(heading);
	y = length * SinDegrees(heading);
}

void Vector2::RotateRadians(float radians)
{
	float length = CalcLength();
	float heading = CalcHeadingRadians();
	heading += radians;
	x = length * (float) cos(heading);
	y = length * (float) sin(heading);
}

float Vector2::Normalize()
{
	float length = CalcLength();
	x /= length;
	y /= length;
	return length;
}

Vector2 Vector2::GetVectorNormal(const Vector2 &vec)
{
	float length = vec.CalcLength();
	return Vector2(vec.x / length, vec.y / length);
}

float Vector2::SetLength(float newLength)
{
	float heading = CalcHeadingDegrees();
	float length = CalcLength();
	x = newLength * CosDegrees(heading);
	y = newLength * SinDegrees(heading);
	return length;
}

IntVector2 Vector2::GetFlooredIntVec2()
{
	return IntVector2((int)floor(x), (int)floor(y));
}

bool Vector2::NearlyEquals(const Vector2& vectorB, float approximationValue)
{
	Vector2 vectorDifference = vectorB - *this;
	vectorDifference.x = fabsf(vectorDifference.x);
	vectorDifference.y = fabsf(vectorDifference.y);
	return vectorDifference.x <= approximationValue && vectorDifference.y <= approximationValue;
}

void Vector2::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
}

void Vector2::ScaleNonUniform(const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::InverseScaleNonUniform(const Vector2& perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

void Vector2::SetHeadingDegrees(float headingDegrees)
{
	float length = CalcLength();
	x = length * CosDegrees(headingDegrees);
	y = length * SinDegrees(headingDegrees);
}

void Vector2::SetHeadingRadians(float headingRadians)
{
	float length = CalcLength();
	x = length * (float) cos(headingRadians);
	y = length * (float) sin(headingRadians);
}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	Normalize();
	y = CosDegrees(headingDegrees);
	x = SinDegrees(headingDegrees);
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	Normalize();
	x = (float) cos(headingRadians);
	y = (float) sin(headingRadians);
}

void Vector2::SetLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	x = newLength * CosDegrees(headingDegrees);
	y = newLength * SinDegrees(headingDegrees);
}

void Vector2::SetLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * (float) cos(headingRadians);
	y = newLength * (float) sin(headingRadians);
}

bool Vector2::operator==(const Vector2& vectorToEqual) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y);
}

bool Vector2::operator!=(const Vector2& vectorToNotEqual) const
{
	return (x != vectorToNotEqual.x) || (y != vectorToNotEqual.y);
}

const Vector2 Vector2::operator+(const Vector2& vectorToAdd) const
{
	return Vector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

const Vector2 Vector2::operator-(const Vector2& vectorToSubtract) const
{
	return Vector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

const Vector2 Vector2::operator*(float scale) const
{
	return Vector2(x * scale, y * scale);
}

const Vector2 Vector2::operator*(const Vector2& perAxisScaleFactors) const
{
	return Vector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

const Vector2 Vector2::operator/(float inverseScale) const
{
	return Vector2(x / inverseScale, y / inverseScale);
}

void Vector2::operator*=(float scale)
{
	x *= scale;
	y *= scale;
}

void Vector2::operator*=(const Vector2& perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::operator+=(const Vector2& vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

void Vector2::operator-=(const Vector2& vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

float CalcDistance(const Vector2& positionA, const Vector2& positionB)
{
	Vector2 distanceVector = positionB - positionA;
	distanceVector *= distanceVector;
	return (float) sqrt(distanceVector.x + distanceVector.y);
}

float CalcAngleBetweenVectors(const Vector2& positionA, const Vector2& positionB)
{
	float angle = (float)atan2(positionA.x - positionB.x, positionA.y - positionB.y);
	angle = ConvertRadiansToDegrees(angle);
	if (angle < 0.f)
		angle += 360.f;
	return angle;
}

float CalcDistanceSquared(const Vector2& posA, const Vector2& posB)
{
	Vector2 distanceVector = posB - posA;
	distanceVector *= distanceVector;
	return distanceVector.x + distanceVector.y;
}

const Vector2 operator*(float scale, const Vector2 & vectorToScale)
{
	return vectorToScale * scale;
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

const Vector2 Vector2::ZERO(0.f, 0.f);
const Vector2 Vector2::ONE(1.f, 1.f);