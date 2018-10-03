//Some code based off of code by Squirrel Eiserloh
#pragma once
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <math.h>
#include <stdio.h>

class Vector2
{
public:
	float x;
	float y;

	static const Vector2 ZERO;
	static const Vector2 ONE;

	Vector2();
	Vector2(const Vector2& copy);
	Vector2(const IntVector2& copy);
	explicit Vector2(int initialX, int initialY);
	explicit Vector2(float initialX, float initialY);

	void GetXY(float& out_x, float& out_y) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
	float CalcHeadingDegrees() const;
	float CalcHeadingRadians() const;
	const float CalcDistanceToVector(const Vector2& vectorB);

	void SetXY( float newX, float newY );
	void Rotate90Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
	float Normalize();
	Vector2 GetVectorNormal(const Vector2 &vec);
	float SetLength(float newLength);
	IntVector2 GetFlooredIntVec2();
	bool NearlyEquals(const Vector2& vectorB, float approximationValue);

	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector2& perAxisDivisors);
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
	void SetUnitLengthAndHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
	void SetLengthAndHeadingRadians(float newLength, float headingRadians);

	bool operator==(const Vector2& vectorToEqual) const;
	bool operator!=(const Vector2& vectorToNotEqual) const;
	const Vector2 operator+(const Vector2& vectorToAdd) const;
	const Vector2 operator-(const Vector2& vectorToSubtract) const;
	const Vector2 operator*(float scale) const;
	const Vector2 operator*(const Vector2& perAxisScaleFactors) const;
	const Vector2 operator/(float inverseScale) const;
	void operator*=(float scale);
	void operator*=(const Vector2& perAxisScaleFactors);
	void operator+=(const Vector2& vectorToAdd);
	void operator-=(const Vector2& vectorToSubtract);

	friend float CalcDistance(const Vector2& positionA, const Vector2& positionB);
	friend float CalcAngleBetweenVectors(const Vector2& positionA, const Vector2& positionB);
	friend float CalcDistanceSquared(const Vector2& posA, const Vector2& posB);
	friend const Vector2 operator*(float scale, const Vector2& vectorToScale);
	friend float DotProduct(const Vector2& a, const Vector2& b);
};