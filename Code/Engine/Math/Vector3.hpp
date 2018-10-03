#pragma once

class Vector2;

class Vector3
{
public:
	float x;
	float y;
	float z;

	static const Vector3 ZERO;
	static const Vector3 RIGHT;
	static const Vector3 UP;
	static const Vector3 FORWARD;

	Vector3();
	Vector3(const Vector2& copy);
	Vector3(const Vector3& copy);
	explicit Vector3(float initialX, float initialY, float initialZ);

	void GetXY(float& out_x, float& out_y, float& out_z) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
	const float CalcDistanceToVector(const Vector3& vectorB);
	Vector3 Absolute();

	void SetXYZ(float newX, float newY, float newZ);
	float Normalize();

	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector3& perAxisDivisors);

	bool operator==(const Vector3& vectorToEqual) const;
	bool operator!=(const Vector3& vectorToNotEqual) const;
	const Vector3 operator+(const Vector3& vectorToAdd) const;
	const Vector3 operator-(const Vector3& vectorToSubtract) const;
	const Vector3 operator*(float scale) const;
	const Vector3 operator*(const Vector3& perAxisScaleFactors) const;
	const Vector3 operator/(float inverseScale) const;
	const Vector3 operator-() const;
	void operator*=(float scale);
	void operator*=(const Vector3& perAxisScaleFactors);
	void operator+=(const Vector3& vectorToAdd);
	void operator-=(const Vector3& vectorToSubtract);

	inline Vector3 Cross(Vector3 const &other) const
	{
		return Vector3(
			y * other.z - z * other.y,
			-x * other.z + z * other.x,
			x * other.y - y * other.x
			);
	}

	inline Vector3 GetNormalized() const
	{
		float const il = 1.0f / CalcLength();
		return Vector3(x * il, y * il, z * il);
	}

	inline float Dot(Vector3 const &other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	friend float CalcDistance(const Vector3& positionA, const Vector3& positionB);
	friend float CalcDistanceSquared(const Vector3& posA, const Vector3& posB);
	friend const Vector3 operator*(float scale, const Vector3& vectorToScale);
	friend float DotProduct(const Vector3& a, const Vector3& b);
};