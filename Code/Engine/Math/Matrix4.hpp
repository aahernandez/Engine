#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class Matrix4
{
public:
	Vector4 m_iBasis;
	Vector4 m_jBasis;
	Vector4 m_kBasis;
	Vector4 m_translation;

	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;

	Matrix4();
	Matrix4(const Matrix4& copy);
	Matrix4(const float* arrayOfFloats);
	Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));
	Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation = Vector4(0.f, 0.f, 0.f, 1.f));

	const Matrix4 GetTransformed(const Matrix4& transformMatrix) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();

	void SetTranslation(Vector3 const &pos);
	Vector3 GetTranslation() const;
	Matrix4 GetInverseOrthonormal() const;
	void InvertOrthonormal();
	Vector4 GetColumn(unsigned int columnNum);

	void MakeIdentity();
	Matrix4 GetInverse(Matrix4 mat);
	Matrix4 GetInverse() const;
	float GetDeterminate() const;
	void ConcatenateTransform(const Matrix4& transformMatrix);
	Vector3 Transform( Vector3 const &vec );
	Vector2 TransformPosition(const Vector2& position2D) const;
	Vector3 TransformPosition(const Vector3& position3D) const;
	Vector2 TransformDirection(const Vector2& direction2D) const;
	Vector3 TransformDirection(const Vector3& direction3D) const;
	Vector4 TransformVector(const Vector4& homogeneousVector) const;
	void SetMatrixTranslationAndRotation(Vector3 translation, float rollAboutX, float pitchAboutY, float yawAboutZ);

	void Translate(const Vector2& translation2D);
	void Translate(const Vector3& translation3D);
	void Scale(float uniformScale);
	void Scale(const Vector2& nonUniformScale2D);
	void Scale(const Vector3& nonUniformScale3D);
	void Scale(const Vector4& nonUniformScale4D);
	void RotateDegreesAboutX(float degrees);
	void RotateDegreesAboutY(float degrees);
	void RotateDegreesAboutZ(float degrees);
	void RotateRadiansAboutX(float radians);
	void RotateRadiansAboutY(float radians);
	void RotateRadiansAboutZ(float radians);
	void Transpose();
	bool IsOrthogonal();
	bool IsIdentity();
	Matrix4 Multiply(Matrix4 matrixB);

	void FlipXValues();

	const Matrix4 operator=(const Matrix4& matrixB);
	bool operator==(const Matrix4& vectorToEqual) const;
	bool operator!=(const Matrix4& compareMatrix) const;
	Matrix4 operator*(Matrix4& matrixB);
	Matrix4 operator*(const Matrix4& matrixB);
	Matrix4 operator*(const float &floatA);
	Matrix4 operator-();

	static Matrix4 CreateTranslation(const Vector2& translation2D);
	static Matrix4 CreateTranslation(const Vector3& translation3D);
	static Matrix4 CreateScale(float uniformScale);
	static Matrix4 CreateScale(const Vector2& nonUniformScale2D);
	static Matrix4 CreateScale(const Vector3& nonUniformScale3D);
	static Matrix4 CreateRotationDegreesAboutX(float degrees);
	static Matrix4 CreateRotationDegreesAboutY(float degrees);
	static Matrix4 CreateRotationDegreesAboutZ(float degrees);
	static Matrix4 CreateRotationRadiansAboutX(float radians);
	static Matrix4 CreateRotationRadiansAboutY(float radians);
	static Matrix4 CreateRotationRadiansAboutZ(float radians);
	static Matrix4 MakeOrthoProjection(float nearX, float farX, float nearY, float farY, float nearZ = 0.f, float farZ = 1.f);
	static Matrix4 CreateFromRows(const Vector4 &row1, const Vector4 &row2, const Vector4 &row3, const Vector4 &row4);
	static Matrix4 CreateFromBasis(const Vector3 &basis1, const Vector3 &basis2, const Vector3 &basis3);
};
