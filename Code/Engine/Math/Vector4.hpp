#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/RGBA.hpp"

class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4();
	Vector4(const Vector4& copy);
	Vector4(const Vector3& copy, float initialW);
	explicit Vector4(float initialX, float initialY, float initialZ, float initialW);

	void GetXY(float& out_x, float& out_y, float& out_z, float& out_w) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
	const float CalcDistanceToVector(const Vector4& vectorB);

	void SetXYZ(float newX, float newY, float newZ);
	void SetXYZW(float newX, float newY, float newZ, float newW);
	float Normalize3D();
	float Normalize4D();

	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector4& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector4& perAxisDivisors);

	inline Vector3 xyz() const { return Vector3( x, y, z ); }
	inline Vector2 xy() const { return Vector2(x, y); }

	bool operator==(const Vector4& vectorToEqual) const;
	bool operator!=(const Vector4& vectorToNotEqual) const;
	const Vector4 operator+(const Vector4& vectorToAdd) const;
	const Vector4 operator-(const Vector4& vectorToSubtract) const;
	const Vector4 operator*(float scale) const;
	const Vector4 operator*(const Vector4& perAxisScaleFactors) const;
	const Vector4 operator/(float inverseScale) const;
	void operator*=(float scale);
	void operator*=(const Vector4& perAxisScaleFactors);
	void operator+=(const Vector4& vectorToAdd);
	void operator-=(const Vector4& vectorToSubtract);
	void operator=(const RGBA color);
	Vector4 operator-();

	friend float CalcDistance(const Vector4& positionA, const Vector4& positionB);
	friend float CalcDistanceSquared(const Vector4& posA, const Vector4& posB);
	friend const Vector4 operator*(float scale, const Vector4& vectorToScale);
	friend float DotProduct(const Vector4& a, const Vector4& b);
};