//Some code based off of code by Squirrel Eiserloh
#pragma once
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Math/Vector2.hpp"
#include <math.h>
#include <stdio.h>

class IntVector2
{
public:
	int x;
	int y;

	static const IntVector2 ZERO;

	IntVector2();
	IntVector2(const IntVector2& copy);
	IntVector2(const Vector2& copy);
	IntVector2(float initialX, float initialY);
	explicit IntVector2(int initialX, int initialY);

	void GetXY(int& out_x, int& out_y) const;
	const int* GetAsIntArray() const;
	int* GetAsIntArray();
	int CalcProduct() const;
	int CalcLength() const;
	int CalcLengthSquared() const;
	const int CalcDistanceToIntVector(const IntVector2& vectorB);

	void SetXY(int newX, int newY);

	void ScaleUniform(int scale);
	void ScaleNonUniform(const IntVector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const IntVector2& perAxisDivisors);

	bool operator==(const IntVector2& vectorToEqual) const;
	bool operator!=(const IntVector2& vectorToNotEqual) const;
	const IntVector2 operator+(const IntVector2& vectorToAdd) const;
	const IntVector2 operator-(const IntVector2& vectorToSubtract) const;
	const IntVector2 operator*(int scale) const;
	const IntVector2 operator*(const IntVector2& perAxisScaleFactors) const;
	const IntVector2 operator/(int inverseScale) const;
	void operator*=(int scale);
	void operator*=(const IntVector2& perAxisScaleFactors);
	void operator+=(const IntVector2& vectorToAdd);
	void operator-=(const IntVector2& vectorToSubtract);


	friend bool operator < ( const IntVector2& lhs, const IntVector2& rhs );
	friend int CalcDistance(const IntVector2& positionA, const IntVector2& positionB);
	friend int CalcDistanceSquared(const IntVector2& posA, const IntVector2& posB);
	friend int DotProduct(const IntVector2& a, const IntVector2& b);
};