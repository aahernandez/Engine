//Some code based off of code by Squirrel Eiserloh
#pragma once
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Math/Vector3.hpp"
#include <math.h>
#include <stdio.h>

class IntVector3
{
public:
	int x;
	int y;
	int z;

	IntVector3();
	IntVector3(const IntVector3& copy);
	IntVector3(const Vector3& copy);
	explicit IntVector3(int initialX, int initialY, int initialZ);

	void GetXYZ(int& out_x, int& out_y, int& out_z) const;
	const int* GetAsIntArray() const;
	int* GetAsIntArray();
	int CalcProduct() const;
	int CalcLength() const;
	int CalcLengthSquared() const;
	const int CalcDistanceToIntVector(const IntVector3& vectorB);

	void SetXYZ(int newX, int newY, int newZ);

	void ScaleUniform(int scale);
	void ScaleNonUniform(const IntVector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const IntVector3& perAxisDivisors);

	bool operator==(const IntVector3& vectorToEqual) const;
	bool operator!=(const IntVector3& vectorToNotEqual) const;
	const IntVector3 operator+(const IntVector3& vectorToAdd) const;
	const IntVector3 operator-(const IntVector3& vectorToSubtract) const;
	const IntVector3 operator*(int scale) const;
	const IntVector3 operator*(const IntVector3& perAxisScaleFactors) const;
	const IntVector3 operator/(int inverseScale) const;
	void operator*=(int scale);
	void operator*=(const IntVector3& perAxisScaleFactors);
	void operator+=(const IntVector3& vectorToAdd);
	void operator-=(const IntVector3& vectorToSubtract);

	friend int CalcDistance(const IntVector3& positionA, const IntVector3& positionB);
	friend int CalcDistanceSquared(const IntVector3& posA, const IntVector3& posB);
	friend int DotProduct(const IntVector3& a, const IntVector3& b);
};