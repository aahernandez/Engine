#include "Engine/Math/Matrix4.hpp"
#include "Engine/Core/CommonUtilities.hpp"

Matrix4::Matrix4()
{
	MakeIdentity();
}

Matrix4::Matrix4(const Matrix4& copy)
{
	m_iBasis = copy.m_iBasis;
	m_jBasis = copy.m_jBasis;
	m_kBasis = copy.m_kBasis;
	m_translation = copy.m_translation;
}

Matrix4::Matrix4(const float* arrayOfFloats)
{
	m_iBasis.x = arrayOfFloats[0];
	m_iBasis.y = arrayOfFloats[1];
	m_iBasis.z = arrayOfFloats[2];
	m_iBasis.w = arrayOfFloats[3];

	m_jBasis.x = arrayOfFloats[4];
	m_jBasis.y = arrayOfFloats[5];
	m_jBasis.z = arrayOfFloats[6];
	m_jBasis.w = arrayOfFloats[7];

	m_kBasis.x = arrayOfFloats[8];
	m_kBasis.y = arrayOfFloats[9];
	m_kBasis.z = arrayOfFloats[10];
	m_kBasis.w = arrayOfFloats[11];

	m_translation.x = arrayOfFloats[12];
	m_translation.y = arrayOfFloats[13];
	m_translation.z = arrayOfFloats[14];
	m_translation.w = arrayOfFloats[15];
}

Matrix4::Matrix4(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation /*= Vector2(0.f, 0.f)*/)
{
	MakeIdentity();
	
	m_iBasis.x = iBasis.x;
	m_iBasis.y = iBasis.y;

	m_jBasis.x = jBasis.x;
	m_jBasis.y = jBasis.y;

	m_translation.x = translation.x;
	m_translation.y = translation.y;
}

Matrix4::Matrix4(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation /*= Vector3(0.f, 0.f, 0.f)*/)
{
	MakeIdentity();

	m_iBasis.x = iBasis.x;
	m_iBasis.y = iBasis.y;
	m_iBasis.z = iBasis.z;

	m_jBasis.x = jBasis.x;
	m_jBasis.y = jBasis.y;
	m_jBasis.z = jBasis.z;

	m_kBasis.x = kBasis.x;
	m_kBasis.y = kBasis.y;
	m_kBasis.z = kBasis.z;

	m_translation.x = translation.x;
	m_translation.y = translation.y;
	m_translation.z = translation.z;
}

Matrix4::Matrix4(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation /*= Vector4(0.f, 0.f, 0.f, 1.f)*/)
{
	m_iBasis = iBasis;
	m_jBasis = jBasis;
	m_kBasis = kBasis;
	m_translation = translation;
}

void Matrix4::MakeIdentity()
{
	m_iBasis = Vector4(1.f, 0.f, 0.f, 0.f);
	m_jBasis = Vector4(0.f, 1.f, 0.f, 0.f);
	m_kBasis = Vector4(0.f, 0.f, 1.f, 0.f);
	m_translation = Vector4(0.f, 0.f, 0.f, 1.f);
}

Matrix4 Matrix4::GetInverse(Matrix4 mat)
{
	float determinate = mat.GetDeterminate();
	Matrix4 inverseMatrix;

	inverseMatrix = mat * (float) (1.f / determinate);
	return inverseMatrix;
}

void Matrix4::FlipXValues()
{
	m_iBasis.x = -m_iBasis.x;
	m_jBasis.x = -m_jBasis.x;
	m_kBasis.x = -m_kBasis.x;
	m_translation.x = -m_translation.x;
}

Matrix4 Matrix4::GetInverse() const
{
	float inv[16];
	const float *matrixAsArray = GetAsFloatArray();
	float det;

	inv[0] = matrixAsArray[5] * matrixAsArray[10] * matrixAsArray[15] -
		matrixAsArray[5] * matrixAsArray[11] * matrixAsArray[14] -
		matrixAsArray[9] * matrixAsArray[6] * matrixAsArray[15] +
		matrixAsArray[9] * matrixAsArray[7] * matrixAsArray[14] +
		matrixAsArray[13] * matrixAsArray[6] * matrixAsArray[11] -
		matrixAsArray[13] * matrixAsArray[7] * matrixAsArray[10];

	inv[4] = -matrixAsArray[4] * matrixAsArray[10] * matrixAsArray[15] +
		matrixAsArray[4] * matrixAsArray[11] * matrixAsArray[14] +
		matrixAsArray[8] * matrixAsArray[6] * matrixAsArray[15] -
		matrixAsArray[8] * matrixAsArray[7] * matrixAsArray[14] -
		matrixAsArray[12] * matrixAsArray[6] * matrixAsArray[11] +
		matrixAsArray[12] * matrixAsArray[7] * matrixAsArray[10];

	inv[8] = matrixAsArray[4] * matrixAsArray[9] * matrixAsArray[15] -
		matrixAsArray[4] * matrixAsArray[11] * matrixAsArray[13] -
		matrixAsArray[8] * matrixAsArray[5] * matrixAsArray[15] +
		matrixAsArray[8] * matrixAsArray[7] * matrixAsArray[13] +
		matrixAsArray[12] * matrixAsArray[5] * matrixAsArray[11] -
		matrixAsArray[12] * matrixAsArray[7] * matrixAsArray[9];

	inv[12] = -matrixAsArray[4] * matrixAsArray[9] * matrixAsArray[14] +
		matrixAsArray[4] * matrixAsArray[10] * matrixAsArray[13] +
		matrixAsArray[8] * matrixAsArray[5] * matrixAsArray[14] -
		matrixAsArray[8] * matrixAsArray[6] * matrixAsArray[13] -
		matrixAsArray[12] * matrixAsArray[5] * matrixAsArray[10] +
		matrixAsArray[12] * matrixAsArray[6] * matrixAsArray[9];

	inv[1] = -matrixAsArray[1] * matrixAsArray[10] * matrixAsArray[15] +
		matrixAsArray[1] * matrixAsArray[11] * matrixAsArray[14] +
		matrixAsArray[9] * matrixAsArray[2] * matrixAsArray[15] -
		matrixAsArray[9] * matrixAsArray[3] * matrixAsArray[14] -
		matrixAsArray[13] * matrixAsArray[2] * matrixAsArray[11] +
		matrixAsArray[13] * matrixAsArray[3] * matrixAsArray[10];

	inv[5] = matrixAsArray[0] * matrixAsArray[10] * matrixAsArray[15] -
		matrixAsArray[0] * matrixAsArray[11] * matrixAsArray[14] -
		matrixAsArray[8] * matrixAsArray[2] * matrixAsArray[15] +
		matrixAsArray[8] * matrixAsArray[3] * matrixAsArray[14] +
		matrixAsArray[12] * matrixAsArray[2] * matrixAsArray[11] -
		matrixAsArray[12] * matrixAsArray[3] * matrixAsArray[10];

	inv[9] = -matrixAsArray[0] * matrixAsArray[9] * matrixAsArray[15] +
		matrixAsArray[0] * matrixAsArray[11] * matrixAsArray[13] +
		matrixAsArray[8] * matrixAsArray[1] * matrixAsArray[15] -
		matrixAsArray[8] * matrixAsArray[3] * matrixAsArray[13] -
		matrixAsArray[12] * matrixAsArray[1] * matrixAsArray[11] +
		matrixAsArray[12] * matrixAsArray[3] * matrixAsArray[9];

	inv[13] = matrixAsArray[0] * matrixAsArray[9] * matrixAsArray[14] -
		matrixAsArray[0] * matrixAsArray[10] * matrixAsArray[13] -
		matrixAsArray[8] * matrixAsArray[1] * matrixAsArray[14] +
		matrixAsArray[8] * matrixAsArray[2] * matrixAsArray[13] +
		matrixAsArray[12] * matrixAsArray[1] * matrixAsArray[10] -
		matrixAsArray[12] * matrixAsArray[2] * matrixAsArray[9];

	inv[2] = matrixAsArray[1] * matrixAsArray[6] * matrixAsArray[15] -
		matrixAsArray[1] * matrixAsArray[7] * matrixAsArray[14] -
		matrixAsArray[5] * matrixAsArray[2] * matrixAsArray[15] +
		matrixAsArray[5] * matrixAsArray[3] * matrixAsArray[14] +
		matrixAsArray[13] * matrixAsArray[2] * matrixAsArray[7] -
		matrixAsArray[13] * matrixAsArray[3] * matrixAsArray[6];

	inv[6] = -matrixAsArray[0] * matrixAsArray[6] * matrixAsArray[15] +
		matrixAsArray[0] * matrixAsArray[7] * matrixAsArray[14] +
		matrixAsArray[4] * matrixAsArray[2] * matrixAsArray[15] -
		matrixAsArray[4] * matrixAsArray[3] * matrixAsArray[14] -
		matrixAsArray[12] * matrixAsArray[2] * matrixAsArray[7] +
		matrixAsArray[12] * matrixAsArray[3] * matrixAsArray[6];

	inv[10] = matrixAsArray[0] * matrixAsArray[5] * matrixAsArray[15] -
		matrixAsArray[0] * matrixAsArray[7] * matrixAsArray[13] -
		matrixAsArray[4] * matrixAsArray[1] * matrixAsArray[15] +
		matrixAsArray[4] * matrixAsArray[3] * matrixAsArray[13] +
		matrixAsArray[12] * matrixAsArray[1] * matrixAsArray[7] -
		matrixAsArray[12] * matrixAsArray[3] * matrixAsArray[5];

	inv[14] = -matrixAsArray[0] * matrixAsArray[5] * matrixAsArray[14] +
		matrixAsArray[0] * matrixAsArray[6] * matrixAsArray[13] +
		matrixAsArray[4] * matrixAsArray[1] * matrixAsArray[14] -
		matrixAsArray[4] * matrixAsArray[2] * matrixAsArray[13] -
		matrixAsArray[12] * matrixAsArray[1] * matrixAsArray[6] +
		matrixAsArray[12] * matrixAsArray[2] * matrixAsArray[5];

	inv[3] = -matrixAsArray[1] * matrixAsArray[6] * matrixAsArray[11] +
		matrixAsArray[1] * matrixAsArray[7] * matrixAsArray[10] +
		matrixAsArray[5] * matrixAsArray[2] * matrixAsArray[11] -
		matrixAsArray[5] * matrixAsArray[3] * matrixAsArray[10] -
		matrixAsArray[9] * matrixAsArray[2] * matrixAsArray[7] +
		matrixAsArray[9] * matrixAsArray[3] * matrixAsArray[6];

	inv[7] = matrixAsArray[0] * matrixAsArray[6] * matrixAsArray[11] -
		matrixAsArray[0] * matrixAsArray[7] * matrixAsArray[10] -
		matrixAsArray[4] * matrixAsArray[2] * matrixAsArray[11] +
		matrixAsArray[4] * matrixAsArray[3] * matrixAsArray[10] +
		matrixAsArray[8] * matrixAsArray[2] * matrixAsArray[7] -
		matrixAsArray[8] * matrixAsArray[3] * matrixAsArray[6];

	inv[11] = -matrixAsArray[0] * matrixAsArray[5] * matrixAsArray[11] +
		matrixAsArray[0] * matrixAsArray[7] * matrixAsArray[9] +
		matrixAsArray[4] * matrixAsArray[1] * matrixAsArray[11] -
		matrixAsArray[4] * matrixAsArray[3] * matrixAsArray[9] -
		matrixAsArray[8] * matrixAsArray[1] * matrixAsArray[7] +
		matrixAsArray[8] * matrixAsArray[3] * matrixAsArray[5];

	inv[15] = matrixAsArray[0] * matrixAsArray[5] * matrixAsArray[10] -
		matrixAsArray[0] * matrixAsArray[6] * matrixAsArray[9] -
		matrixAsArray[4] * matrixAsArray[1] * matrixAsArray[10] +
		matrixAsArray[4] * matrixAsArray[2] * matrixAsArray[9] +
		matrixAsArray[8] * matrixAsArray[1] * matrixAsArray[6] -
		matrixAsArray[8] * matrixAsArray[2] * matrixAsArray[5];

	det = matrixAsArray[0] * inv[0] + matrixAsArray[1] * inv[4] + matrixAsArray[2] * inv[8] + matrixAsArray[3] * inv[12];

	if (det == 0)
		return Matrix4::IDENTITY;

	det = 1.f / det;

	float invOut[16];

	for (int i = 0; i < 16; i++)
	{
		float newValue = inv[i] * det;
		invOut[i] = newValue;
	}

	return Matrix4(invOut);
}

float Matrix4::GetDeterminate() const
{
	float determinate;

	determinate = m_iBasis.x * m_jBasis.y * m_kBasis.z * m_translation.w;
	determinate += m_iBasis.x * m_jBasis.z * m_kBasis.w * m_translation.y;
	determinate += m_iBasis.x * m_jBasis.w * m_kBasis.y * m_translation.z;
	determinate += m_iBasis.y * m_jBasis.x * m_kBasis.w * m_translation.z;
	determinate += m_iBasis.y * m_jBasis.z * m_kBasis.x * m_translation.w;
	determinate += m_iBasis.y * m_jBasis.w * m_kBasis.z * m_translation.x;
	determinate += m_iBasis.z * m_jBasis.x * m_kBasis.y * m_translation.w;
	determinate += m_iBasis.z * m_jBasis.y * m_kBasis.w * m_translation.x;
	determinate += m_iBasis.z * m_jBasis.w * m_kBasis.x * m_translation.y;
	determinate += m_iBasis.w * m_jBasis.x * m_kBasis.z * m_translation.y;
	determinate += m_iBasis.w * m_jBasis.y * m_kBasis.x * m_translation.z;
	determinate += m_iBasis.w * m_jBasis.z * m_kBasis.y * m_translation.x;

	determinate -= m_iBasis.x * m_jBasis.y * m_kBasis.w * m_translation.z;
	determinate -= m_iBasis.x * m_jBasis.z * m_kBasis.y * m_translation.w;
	determinate -= m_iBasis.x * m_jBasis.w * m_kBasis.z * m_translation.y;
	determinate -= m_iBasis.y * m_jBasis.x * m_kBasis.z * m_translation.w;
	determinate -= m_iBasis.y * m_jBasis.z * m_kBasis.w * m_translation.x;
	determinate -= m_iBasis.y * m_jBasis.w * m_kBasis.x * m_translation.z;
	determinate -= m_iBasis.z * m_jBasis.x * m_kBasis.w * m_translation.y;
	determinate -= m_iBasis.z * m_jBasis.y * m_kBasis.x * m_translation.w;
	determinate -= m_iBasis.z * m_jBasis.w * m_kBasis.y * m_translation.x;
	determinate -= m_iBasis.w * m_jBasis.x * m_kBasis.y * m_translation.z;
	determinate -= m_iBasis.w * m_jBasis.y * m_kBasis.z * m_translation.x;
	determinate -= m_iBasis.w * m_jBasis.z * m_kBasis.x * m_translation.w;

	return determinate;
}

void Matrix4::ConcatenateTransform(const Matrix4& transformMatrix)
{
	Matrix4 concatenatedMatrix = GetTransformed(transformMatrix);

	m_iBasis = concatenatedMatrix.m_iBasis;
	m_jBasis = concatenatedMatrix.m_jBasis;
	m_kBasis = concatenatedMatrix.m_kBasis;
	m_translation = concatenatedMatrix.m_translation;
}

Vector3 Matrix4::Transform(Vector3 const &vec)
{
	Vector3 returnVec;
	returnVec.x = vec.Dot(GetColumn(0).xyz() );
	returnVec.y = vec.Dot(GetColumn(1).xyz() );
	returnVec.z = vec.Dot(GetColumn(2).xyz() );

	return returnVec;
}

Vector2 Matrix4::TransformPosition(const Vector2& position2D) const
{
	Vector2 transformedVector;
	transformedVector.x = (m_iBasis.x * position2D.x)
		+ (m_jBasis.x * position2D.y)
		+ (m_kBasis.x * 0.f)
		+ (m_translation.x * 1.f);

	transformedVector.y = (m_iBasis.y * position2D.x)
		+ (m_jBasis.y * position2D.y)
		+ (m_kBasis.y * 0.f)
		+ (m_translation.y * 1.f);

	return transformedVector;
}

Vector3 Matrix4::TransformPosition(const Vector3& position3D) const
{
// 	Matrix4 transpose = *this;
// 	transpose.Transpose();
	
	Vector3 transformedVector;
// 	transformedVector.x = (m_iBasis.x * position3D.x)
// 		+ (m_iBasis.y * position3D.y)
// 		+ (m_iBasis.z * position3D.z)
// 		+ (m_iBasis.w * 1.f);
// 
// 	transformedVector.y = (m_jBasis.x * position3D.x)
// 		+ (m_jBasis.y * position3D.y)
// 		+ (m_jBasis.z * position3D.z)
// 		+ (m_jBasis.w * 1.f);
// 
// 	transformedVector.z = (m_kBasis.x * position3D.x)
// 		+ (m_kBasis.y * position3D.y)
// 		+ (m_kBasis.z * position3D.z)
// 		+ (m_kBasis.w * 1.f);

	transformedVector.x = (m_iBasis.x * position3D.x)
		+ (m_jBasis.x * position3D.y)
		+ (m_kBasis.x * position3D.z)
		+ (m_translation.x * 1.f);

	transformedVector.y = (m_iBasis.y * position3D.x)
		+ (m_jBasis.y * position3D.y)
		+ (m_kBasis.y * position3D.z)
		+ (m_translation.y * 1.f);

	transformedVector.z = (m_iBasis.z * position3D.x)
		+ (m_jBasis.z * position3D.y)
		+ (m_kBasis.z * position3D.z)
		+ (m_translation.z * 1.f);

	return transformedVector;
}

Vector2 Matrix4::TransformDirection(const Vector2& direction2D) const
{
	Vector2 transformedVector;
	transformedVector.x = (m_iBasis.x * direction2D.x)
		+ (m_jBasis.x * direction2D.y)
		+ (m_kBasis.x * 0.f)
		+ (m_translation.x * 0.f);

	transformedVector.y = (m_iBasis.y * direction2D.x)
		+ (m_jBasis.y * direction2D.y)
		+ (m_kBasis.y * 0.f)
		+ (m_translation.y * 0.f);

	return transformedVector;
}

Vector3 Matrix4::TransformDirection(const Vector3& direction3D) const
{
	Vector3 transformedVector;
	transformedVector.x = (m_iBasis.x * direction3D.x)
		+ (m_jBasis.x * direction3D.y)
		+ (m_kBasis.x * direction3D.z)
		+ (m_translation.x * 0.f);

	transformedVector.y = (m_iBasis.y * direction3D.x)
		+ (m_jBasis.y * direction3D.y)
		+ (m_kBasis.y * direction3D.z)
		+ (m_translation.y * 0.f);

	transformedVector.z = (m_iBasis.z * direction3D.x)
		+ (m_jBasis.z * direction3D.y)
		+ (m_kBasis.z * direction3D.z)
		+ (m_translation.z * 0.f);

	return transformedVector;
}

Vector4 Matrix4::TransformVector(const Vector4& homogeneousVector) const
{
	Vector4 transformedVector;
	transformedVector.x = (m_iBasis.x * homogeneousVector.x)
		+ (m_jBasis.x * homogeneousVector.y)
		+ (m_kBasis.x * homogeneousVector.z)
		+ (m_translation.x * homogeneousVector.w);

	transformedVector.y = (m_iBasis.y * homogeneousVector.x)
		+ (m_jBasis.y * homogeneousVector.y)
		+ (m_kBasis.y * homogeneousVector.z)
		+ (m_translation.y * homogeneousVector.w);

	transformedVector.z = (m_iBasis.z * homogeneousVector.x)
		+ (m_jBasis.z * homogeneousVector.y)
		+ (m_kBasis.z * homogeneousVector.z)
		+ (m_translation.z * homogeneousVector.w);

	transformedVector.w = (m_iBasis.z * homogeneousVector.x)
		+ (m_jBasis.w * homogeneousVector.y)
		+ (m_kBasis.w * homogeneousVector.z)
		+ (m_translation.w * homogeneousVector.w);

	return transformedVector;
}

void Matrix4::SetMatrixTranslationAndRotation(Vector3 translation, float rollAboutX, float pitchAboutY, float yawAboutZ)
{
	Matrix4 xMatrix;
	xMatrix.m_iBasis.x = CosDegrees(rollAboutX);
	xMatrix.m_iBasis.y = SinDegrees(rollAboutX);
	xMatrix.m_jBasis.x = -SinDegrees(rollAboutX);
	xMatrix.m_jBasis.y = CosDegrees(rollAboutX);

	Matrix4 yMatrix;
	yMatrix.m_jBasis.y = CosDegrees(pitchAboutY);
	yMatrix.m_jBasis.z = SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.y = -SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.z = CosDegrees(pitchAboutY);

	Matrix4 zMatrix;
	zMatrix.m_iBasis.x = CosDegrees(yawAboutZ);
	zMatrix.m_iBasis.z = -SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.x = SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.z = CosDegrees(yawAboutZ);

	Matrix4 rotationMatrix;

	rotationMatrix.m_translation.x = translation.x;
	rotationMatrix.m_translation.y = translation.y;
	rotationMatrix.m_translation.z = translation.z;

	rotationMatrix = rotationMatrix * zMatrix;
	rotationMatrix = rotationMatrix * yMatrix;
	rotationMatrix = rotationMatrix * xMatrix;

// 	rotationMatrix = rotationMatrix.GetInverse(rotationMatrix);
// 	rotationMatrix.Transpose();
}

void Matrix4::Translate(const Vector2& translation2D)
{
	m_iBasis.x += translation2D.x;
	m_iBasis.y += translation2D.y;

	m_jBasis.x += translation2D.x;
	m_jBasis.y += translation2D.y;

	m_kBasis.x += translation2D.x;
	m_kBasis.y += translation2D.y;
}

void Matrix4::Translate(const Vector3& translation3D)
{
	m_iBasis.x += translation3D.x;
	m_iBasis.y += translation3D.y;
	m_iBasis.z += translation3D.z;

	m_jBasis.x += translation3D.x;
	m_jBasis.y += translation3D.y;
	m_jBasis.z += translation3D.z;

	m_kBasis.x += translation3D.x;
	m_kBasis.y += translation3D.y;
	m_kBasis.z += translation3D.z;
}

void Matrix4::Scale(float uniformScale)
{
	m_iBasis.ScaleUniform(uniformScale);
	m_jBasis.ScaleUniform(uniformScale);
	m_kBasis.ScaleUniform(uniformScale);
	m_translation.ScaleUniform(uniformScale);
}

void Matrix4::Scale(const Vector2& nonUniformScale2D)
{
	Vector4 nonUniformScale4D;
	nonUniformScale4D.x = nonUniformScale2D.x;
	nonUniformScale4D.y = nonUniformScale2D.y;
	nonUniformScale4D.z = 1.f;
	nonUniformScale4D.w = 1.f;

	Scale(nonUniformScale4D);
}

void Matrix4::Scale(const Vector3& nonUniformScale3D)
{
	Vector4 nonUniformScale4D;
	nonUniformScale4D.x = nonUniformScale3D.x;
	nonUniformScale4D.y = nonUniformScale3D.y;
	nonUniformScale4D.z = nonUniformScale3D.z;
	nonUniformScale4D.w = 1.f;

	Scale(nonUniformScale4D);
}

void Matrix4::Scale(const Vector4& nonUniformScale4D)
{
	m_iBasis.ScaleNonUniform(nonUniformScale4D);
	m_jBasis.ScaleNonUniform(nonUniformScale4D);
	m_kBasis.ScaleNonUniform(nonUniformScale4D);
	m_translation.ScaleNonUniform(nonUniformScale4D);
}

void Matrix4::RotateDegreesAboutX(float degrees)
{
	Vector4 newJBasis = (m_jBasis * CosDegrees(degrees)) + (m_kBasis * -SinDegrees(degrees));
	Vector4 newKBasis = (m_jBasis * SinDegrees(degrees)) + (m_kBasis * CosDegrees(degrees));

	m_jBasis = newJBasis;
	m_kBasis = newKBasis;
}

void Matrix4::RotateDegreesAboutY(float degrees)
{
	Vector4 newIBasis = (m_iBasis * CosDegrees(degrees)) + (m_kBasis * SinDegrees(degrees));
	Vector4 newKBasis = (m_iBasis * -SinDegrees(degrees)) + (m_kBasis * CosDegrees(degrees));

	m_iBasis = newIBasis;
	m_kBasis = newKBasis;
}

void Matrix4::RotateDegreesAboutZ(float degrees)
{
	Vector4 newIBasis = (m_iBasis * CosDegrees(degrees)) + (m_jBasis * -SinDegrees(degrees));
	Vector4 newJBasis = (m_iBasis * SinDegrees(degrees)) + (m_jBasis * CosDegrees(degrees));

	m_iBasis = newIBasis;
	m_jBasis = newJBasis;
}

void Matrix4::RotateRadiansAboutX(float radians)
{
	m_jBasis.y *= (float) cos(radians);
	m_jBasis.z *= (float) sin(radians);

	m_kBasis.y *= (float) -sin(radians);
	m_kBasis.z *= (float) cos(radians);
}

void Matrix4::RotateRadiansAboutY(float radians)
{
	m_iBasis.x *= (float) cos(radians);
	m_iBasis.z *= (float) -sin(radians);

	m_kBasis.x *= (float) sin(radians);
	m_kBasis.z *= (float) cos(radians);
}

void Matrix4::RotateRadiansAboutZ(float radians)
{
	m_iBasis.x *= (float) cos(radians);
	m_iBasis.y *= (float) sin(radians);

	m_jBasis.x *= (float) -sin(radians);
	m_jBasis.y *= (float) cos(radians);
}

void Matrix4::Transpose()
{
	SwapValues(m_iBasis.y, m_jBasis.x);
	SwapValues(m_iBasis.z, m_kBasis.x);
	SwapValues(m_iBasis.w, m_translation.x);
	SwapValues(m_jBasis.z, m_kBasis.y);
	SwapValues(m_jBasis.w, m_translation.y);
	SwapValues(m_kBasis.w, m_translation.z);
}

Matrix4 Matrix4::MakeOrthoProjection(float nearX, float farX, float nearY, float farY, float nearZ /*= 0.f*/, float farZ /*= 1.f*/)
{
	Matrix4 mat;

	float sx = 1.f / (farX - nearX);
	float sy = 1.f / (farY - nearY);
	float sz = 1.f / (farZ - nearZ);

	mat.m_iBasis = Vector4(2.f * sx,			0.f,				0.f,		0.f);
	mat.m_jBasis = Vector4( 0.f,				2.f * sy,			0.f,		0.f);
	mat.m_kBasis = Vector4( 0.f,				0.f,				1.f * sz,	0.f);
	mat.m_translation = Vector4( -(farX + nearX) * sx,	-(farY + nearY) * sy,	-nearZ * sz,	1.f);

	return mat;
}

const Matrix4 Matrix4::GetTransformed(const Matrix4& transformMatrix) const
{
	Matrix4 concatenatedMatrix;
	concatenatedMatrix.m_iBasis.x = (m_iBasis.x * transformMatrix.m_iBasis.x)
		+ (m_jBasis.x * transformMatrix.m_iBasis.y)
		+ (m_kBasis.x * transformMatrix.m_iBasis.z)
		+ (m_translation.x * transformMatrix.m_iBasis.w);

	concatenatedMatrix.m_iBasis.y = (m_iBasis.y * transformMatrix.m_iBasis.x)
		+ (m_jBasis.y * transformMatrix.m_iBasis.y)
		+ (m_kBasis.y * transformMatrix.m_iBasis.z)
		+ (m_translation.y * transformMatrix.m_iBasis.w);

	concatenatedMatrix.m_iBasis.z = (m_iBasis.z * transformMatrix.m_iBasis.x)
		+ (m_jBasis.z * transformMatrix.m_iBasis.y)
		+ (m_kBasis.z * transformMatrix.m_iBasis.z)
		+ (m_translation.z * transformMatrix.m_iBasis.w);

	concatenatedMatrix.m_iBasis.w = (m_iBasis.w * transformMatrix.m_iBasis.x)
		+ (m_jBasis.w * transformMatrix.m_iBasis.y)
		+ (m_kBasis.w * transformMatrix.m_iBasis.z)
		+ (m_translation.w * transformMatrix.m_iBasis.w);


	concatenatedMatrix.m_jBasis.x = (m_iBasis.x * transformMatrix.m_jBasis.x)
		+ (m_jBasis.x * transformMatrix.m_jBasis.y)
		+ (m_kBasis.x * transformMatrix.m_jBasis.z)
		+ (m_translation.x * transformMatrix.m_jBasis.w);

	concatenatedMatrix.m_jBasis.y = (m_iBasis.y * transformMatrix.m_jBasis.x)
		+ (m_jBasis.y * transformMatrix.m_jBasis.y)
		+ (m_kBasis.y * transformMatrix.m_jBasis.z)
		+ (m_translation.y * transformMatrix.m_jBasis.w);

	concatenatedMatrix.m_jBasis.z = (m_iBasis.z * transformMatrix.m_jBasis.x)
		+ (m_jBasis.z * transformMatrix.m_jBasis.y)
		+ (m_kBasis.z * transformMatrix.m_jBasis.z)
		+ (m_translation.z * transformMatrix.m_jBasis.w);

	concatenatedMatrix.m_jBasis.w = (m_iBasis.w * transformMatrix.m_jBasis.x)
		+ (m_jBasis.w * transformMatrix.m_jBasis.y)
		+ (m_kBasis.w * transformMatrix.m_jBasis.z)
		+ (m_translation.w * transformMatrix.m_jBasis.w);


	concatenatedMatrix.m_kBasis.x = (m_iBasis.x * transformMatrix.m_kBasis.x)
		+ (m_jBasis.x * transformMatrix.m_kBasis.y)
		+ (m_kBasis.x * transformMatrix.m_kBasis.z)
		+ (m_translation.x * transformMatrix.m_kBasis.w);

	concatenatedMatrix.m_kBasis.y = (m_iBasis.y * transformMatrix.m_kBasis.x)
		+ (m_jBasis.y * transformMatrix.m_kBasis.y)
		+ (m_kBasis.y * transformMatrix.m_kBasis.z)
		+ (m_translation.y * transformMatrix.m_kBasis.w);

	concatenatedMatrix.m_kBasis.z = (m_iBasis.z * transformMatrix.m_kBasis.x)
		+ (m_jBasis.z * transformMatrix.m_kBasis.y)
		+ (m_kBasis.z * transformMatrix.m_kBasis.z)
		+ (m_translation.z * transformMatrix.m_kBasis.w);

	concatenatedMatrix.m_kBasis.w = (m_iBasis.w * transformMatrix.m_kBasis.x)
		+ (m_jBasis.w * transformMatrix.m_kBasis.y)
		+ (m_kBasis.w * transformMatrix.m_kBasis.z)
		+ (m_translation.w * transformMatrix.m_kBasis.w);


	concatenatedMatrix.m_translation.x = (m_iBasis.x * transformMatrix.m_translation.x)
		+ (m_jBasis.x * transformMatrix.m_translation.y)
		+ (m_kBasis.x * transformMatrix.m_translation.z)
		+ (m_translation.x * transformMatrix.m_kBasis.w);

	concatenatedMatrix.m_translation.y = (m_iBasis.y * transformMatrix.m_translation.x)
		+ (m_jBasis.y * transformMatrix.m_translation.y)
		+ (m_kBasis.y * transformMatrix.m_translation.z)
		+ (m_translation.y * transformMatrix.m_translation.w);

	concatenatedMatrix.m_translation.z = (m_iBasis.z * transformMatrix.m_translation.x)
		+ (m_jBasis.z * transformMatrix.m_translation.y)
		+ (m_kBasis.z * transformMatrix.m_translation.z)
		+ (m_translation.z * transformMatrix.m_translation.w);

	concatenatedMatrix.m_translation.w = (m_iBasis.w * transformMatrix.m_translation.x)
		+ (m_jBasis.w * transformMatrix.m_translation.y)
		+ (m_kBasis.w * transformMatrix.m_translation.z)
		+ (m_translation.w * transformMatrix.m_translation.w);

	return concatenatedMatrix;
}

const float* Matrix4::GetAsFloatArray() const
{
	return &m_iBasis.x;
}

float* Matrix4::GetAsFloatArray()
{
	return &m_iBasis.x;
}

void Matrix4::SetTranslation(Vector3 const &pos)
{
	m_translation = Vector4(pos, 1.f);
}

Vector3 Matrix4::GetTranslation() const
{
	return Vector3(m_translation.x, m_translation.y, m_translation.z);
}

Matrix4 Matrix4::GetInverseOrthonormal() const
{
	Matrix4 ret = *this;
	ret.InvertOrthonormal();
	return ret;
}

void Matrix4::InvertOrthonormal()
{
	Vector3 translation = GetTranslation();
	SetTranslation( Vector3() );

	Transpose();

	translation = Transform(translation);
	SetTranslation(-translation);
}

Vector4 Matrix4::GetColumn(unsigned int columnNum)
{
	switch (columnNum) {
	case 0: 
		return Vector4(m_iBasis.x, m_jBasis.x, m_kBasis.x, m_translation.x);
	case 1:									 
		return Vector4(m_iBasis.y, m_jBasis.y, m_kBasis.y, m_translation.y);
	case 2:								
		return Vector4(m_iBasis.z, m_jBasis.z, m_kBasis.z, m_translation.z);
	case 3:									
		return Vector4(m_iBasis.w, m_jBasis.w, m_kBasis.w, m_translation.w);
	}
	return Vector4(m_iBasis.x, m_jBasis.x, m_kBasis.x, m_translation.x);
}

const Matrix4 Matrix4::operator=(const Matrix4& copyMatrix)
{
	m_iBasis = copyMatrix.m_iBasis;
	m_jBasis = copyMatrix.m_jBasis;
	m_kBasis = copyMatrix.m_kBasis;
	m_translation = copyMatrix.m_translation;
	return *this;
}

bool Matrix4::operator==(const Matrix4& vectorToEqual) const
{
	if (m_iBasis == vectorToEqual.m_iBasis && m_jBasis == vectorToEqual.m_jBasis
		&& m_kBasis == vectorToEqual.m_kBasis && m_translation == vectorToEqual.m_translation)
		return true;
	return false;
}

bool Matrix4::operator!=(const Matrix4& compareMatrix) const
{
	return !(*this == compareMatrix);
}

Matrix4 Matrix4::operator*(Matrix4& matrixB)
{
	Matrix4 product;

	product.m_iBasis.x = m_iBasis.x * matrixB.m_iBasis.x + m_iBasis.y * matrixB.m_jBasis.x + m_iBasis.z * matrixB.m_kBasis.x + m_iBasis.w * matrixB.m_translation.x;
	product.m_iBasis.y = m_iBasis.x * matrixB.m_iBasis.y + m_iBasis.y * matrixB.m_jBasis.y + m_iBasis.z * matrixB.m_kBasis.y + m_iBasis.w * matrixB.m_translation.y;
	product.m_iBasis.z = m_iBasis.x * matrixB.m_iBasis.z + m_iBasis.y * matrixB.m_jBasis.z + m_iBasis.z * matrixB.m_kBasis.z + m_iBasis.w * matrixB.m_translation.z;
	product.m_iBasis.w = m_iBasis.x * matrixB.m_iBasis.w + m_iBasis.y * matrixB.m_jBasis.w + m_iBasis.z * matrixB.m_kBasis.w + m_iBasis.w * matrixB.m_translation.w;

	product.m_jBasis.x = m_jBasis.x * matrixB.m_iBasis.x + m_jBasis.y * matrixB.m_jBasis.x + m_jBasis.z * matrixB.m_kBasis.x + m_jBasis.w * matrixB.m_translation.x;
	product.m_jBasis.y = m_jBasis.x * matrixB.m_iBasis.y + m_jBasis.y * matrixB.m_jBasis.y + m_jBasis.z * matrixB.m_kBasis.y + m_jBasis.w * matrixB.m_translation.y;
	product.m_jBasis.z = m_jBasis.x * matrixB.m_iBasis.z + m_jBasis.y * matrixB.m_jBasis.z + m_jBasis.z * matrixB.m_kBasis.z + m_jBasis.w * matrixB.m_translation.z;
	product.m_jBasis.w = m_jBasis.x * matrixB.m_iBasis.w + m_jBasis.y * matrixB.m_jBasis.w + m_jBasis.z * matrixB.m_kBasis.w + m_jBasis.w * matrixB.m_translation.w;

	product.m_kBasis.x = m_kBasis.x * matrixB.m_iBasis.x + m_kBasis.y * matrixB.m_jBasis.x + m_kBasis.z * matrixB.m_kBasis.x + m_kBasis.w * matrixB.m_translation.x;
	product.m_kBasis.y = m_kBasis.x * matrixB.m_iBasis.y + m_kBasis.y * matrixB.m_jBasis.y + m_kBasis.z * matrixB.m_kBasis.y + m_kBasis.w * matrixB.m_translation.y;
	product.m_kBasis.z = m_kBasis.x * matrixB.m_iBasis.z + m_kBasis.y * matrixB.m_jBasis.z + m_kBasis.z * matrixB.m_kBasis.z + m_kBasis.w * matrixB.m_translation.z;
	product.m_kBasis.w = m_kBasis.x * matrixB.m_iBasis.w + m_kBasis.y * matrixB.m_jBasis.w + m_kBasis.z * matrixB.m_kBasis.w + m_kBasis.w * matrixB.m_translation.w;

	product.m_translation.x = m_translation.x * matrixB.m_iBasis.x + m_translation.y * matrixB.m_jBasis.x + m_translation.z * matrixB.m_kBasis.x + m_translation.w * matrixB.m_translation.x;
	product.m_translation.y = m_translation.x * matrixB.m_iBasis.y + m_translation.y * matrixB.m_jBasis.y + m_translation.z * matrixB.m_kBasis.y + m_translation.w * matrixB.m_translation.y;
	product.m_translation.z = m_translation.x * matrixB.m_iBasis.z + m_translation.y * matrixB.m_jBasis.z + m_translation.z * matrixB.m_kBasis.z + m_translation.w * matrixB.m_translation.z;
	product.m_translation.w = m_translation.x * matrixB.m_iBasis.w + m_translation.y * matrixB.m_jBasis.w + m_translation.z * matrixB.m_kBasis.w + m_translation.w * matrixB.m_translation.w;

	return product;
}

Matrix4 Matrix4::operator*(const Matrix4& matrixB)
{
	Matrix4 product;

	product.m_iBasis.x = m_iBasis.x * matrixB.m_iBasis.x + m_iBasis.y * matrixB.m_jBasis.x + m_iBasis.z * matrixB.m_kBasis.x + m_iBasis.w * matrixB.m_translation.x;
	product.m_iBasis.y = m_iBasis.x * matrixB.m_iBasis.y + m_iBasis.y * matrixB.m_jBasis.y + m_iBasis.z * matrixB.m_kBasis.y + m_iBasis.w * matrixB.m_translation.y;
	product.m_iBasis.z = m_iBasis.x * matrixB.m_iBasis.z + m_iBasis.y * matrixB.m_jBasis.z + m_iBasis.z * matrixB.m_kBasis.z + m_iBasis.w * matrixB.m_translation.z;
	product.m_iBasis.w = m_iBasis.x * matrixB.m_iBasis.w + m_iBasis.y * matrixB.m_jBasis.w + m_iBasis.z * matrixB.m_kBasis.w + m_iBasis.w * matrixB.m_translation.w;

	product.m_jBasis.x = m_jBasis.x * matrixB.m_iBasis.x + m_jBasis.y * matrixB.m_jBasis.x + m_jBasis.z * matrixB.m_kBasis.x + m_jBasis.w * matrixB.m_translation.x;
	product.m_jBasis.y = m_jBasis.x * matrixB.m_iBasis.y + m_jBasis.y * matrixB.m_jBasis.y + m_jBasis.z * matrixB.m_kBasis.y + m_jBasis.w * matrixB.m_translation.y;
	product.m_jBasis.z = m_jBasis.x * matrixB.m_iBasis.z + m_jBasis.y * matrixB.m_jBasis.z + m_jBasis.z * matrixB.m_kBasis.z + m_jBasis.w * matrixB.m_translation.z;
	product.m_jBasis.w = m_jBasis.x * matrixB.m_iBasis.w + m_jBasis.y * matrixB.m_jBasis.w + m_jBasis.z * matrixB.m_kBasis.w + m_jBasis.w * matrixB.m_translation.w;

	product.m_kBasis.x = m_kBasis.x * matrixB.m_iBasis.x + m_kBasis.y * matrixB.m_jBasis.x + m_kBasis.z * matrixB.m_kBasis.x + m_kBasis.w * matrixB.m_translation.x;
	product.m_kBasis.y = m_kBasis.x * matrixB.m_iBasis.y + m_kBasis.y * matrixB.m_jBasis.y + m_kBasis.z * matrixB.m_kBasis.y + m_kBasis.w * matrixB.m_translation.y;
	product.m_kBasis.z = m_kBasis.x * matrixB.m_iBasis.z + m_kBasis.y * matrixB.m_jBasis.z + m_kBasis.z * matrixB.m_kBasis.z + m_kBasis.w * matrixB.m_translation.z;
	product.m_kBasis.w = m_kBasis.x * matrixB.m_iBasis.w + m_kBasis.y * matrixB.m_jBasis.w + m_kBasis.z * matrixB.m_kBasis.w + m_kBasis.w * matrixB.m_translation.w;

	product.m_translation.x = m_translation.x * matrixB.m_iBasis.x + m_translation.y * matrixB.m_jBasis.x + m_translation.z * matrixB.m_kBasis.x + m_translation.w * matrixB.m_translation.x;
	product.m_translation.y = m_translation.x * matrixB.m_iBasis.y + m_translation.y * matrixB.m_jBasis.y + m_translation.z * matrixB.m_kBasis.y + m_translation.w * matrixB.m_translation.y;
	product.m_translation.z = m_translation.x * matrixB.m_iBasis.z + m_translation.y * matrixB.m_jBasis.z + m_translation.z * matrixB.m_kBasis.z + m_translation.w * matrixB.m_translation.z;
	product.m_translation.w = m_translation.x * matrixB.m_iBasis.w + m_translation.y * matrixB.m_jBasis.w + m_translation.z * matrixB.m_kBasis.w + m_translation.w * matrixB.m_translation.w;

	return product;
}

Matrix4 Matrix4::operator-()
{
	m_iBasis = -m_iBasis;
	m_jBasis = -m_jBasis;
	m_kBasis = -m_kBasis;
	m_translation = -m_translation;
	return *this;
}

Matrix4 Matrix4::Multiply(Matrix4 matrixB)
{
	Matrix4 product;

	product.m_iBasis.x = m_iBasis.x * matrixB.m_iBasis.x + m_iBasis.y * matrixB.m_jBasis.x + m_iBasis.z * matrixB.m_kBasis.x + m_iBasis.w * matrixB.m_translation.x;
	product.m_iBasis.y = m_iBasis.x * matrixB.m_iBasis.y + m_iBasis.y * matrixB.m_jBasis.y + m_iBasis.z * matrixB.m_kBasis.y + m_iBasis.w * matrixB.m_translation.y;
	product.m_iBasis.z = m_iBasis.x * matrixB.m_iBasis.z + m_iBasis.y * matrixB.m_jBasis.z + m_iBasis.z * matrixB.m_kBasis.z + m_iBasis.w * matrixB.m_translation.z;
	product.m_iBasis.w = m_iBasis.x * matrixB.m_iBasis.w + m_iBasis.y * matrixB.m_jBasis.w + m_iBasis.z * matrixB.m_kBasis.w + m_iBasis.w * matrixB.m_translation.w;

	product.m_jBasis.x = m_jBasis.x * matrixB.m_iBasis.x + m_jBasis.y * matrixB.m_jBasis.x + m_jBasis.z * matrixB.m_kBasis.x + m_jBasis.w * matrixB.m_translation.x;
	product.m_jBasis.y = m_jBasis.x * matrixB.m_iBasis.y + m_jBasis.y * matrixB.m_jBasis.y + m_jBasis.z * matrixB.m_kBasis.y + m_jBasis.w * matrixB.m_translation.y;
	product.m_jBasis.z = m_jBasis.x * matrixB.m_iBasis.z + m_jBasis.y * matrixB.m_jBasis.z + m_jBasis.z * matrixB.m_kBasis.z + m_jBasis.w * matrixB.m_translation.z;
	product.m_jBasis.w = m_jBasis.x * matrixB.m_iBasis.w + m_jBasis.y * matrixB.m_jBasis.w + m_jBasis.z * matrixB.m_kBasis.w + m_jBasis.w * matrixB.m_translation.w;

	product.m_kBasis.x = m_kBasis.x * matrixB.m_iBasis.x + m_kBasis.y * matrixB.m_jBasis.x + m_kBasis.z * matrixB.m_kBasis.x + m_kBasis.w * matrixB.m_translation.x;
	product.m_kBasis.y = m_kBasis.x * matrixB.m_iBasis.y + m_kBasis.y * matrixB.m_jBasis.y + m_kBasis.z * matrixB.m_kBasis.y + m_kBasis.w * matrixB.m_translation.y;
	product.m_kBasis.z = m_kBasis.x * matrixB.m_iBasis.z + m_kBasis.y * matrixB.m_jBasis.z + m_kBasis.z * matrixB.m_kBasis.z + m_kBasis.w * matrixB.m_translation.z;
	product.m_kBasis.w = m_kBasis.x * matrixB.m_iBasis.w + m_kBasis.y * matrixB.m_jBasis.w + m_kBasis.z * matrixB.m_kBasis.w + m_kBasis.w * matrixB.m_translation.w;

	product.m_translation.x = m_translation.x * matrixB.m_iBasis.x + m_translation.y * matrixB.m_jBasis.x + m_translation.z * matrixB.m_kBasis.x + m_translation.w * matrixB.m_translation.x;
	product.m_translation.y = m_translation.x * matrixB.m_iBasis.y + m_translation.y * matrixB.m_jBasis.y + m_translation.z * matrixB.m_kBasis.y + m_translation.w * matrixB.m_translation.y;
	product.m_translation.z = m_translation.x * matrixB.m_iBasis.z + m_translation.y * matrixB.m_jBasis.z + m_translation.z * matrixB.m_kBasis.z + m_translation.w * matrixB.m_translation.z;
	product.m_translation.w = m_translation.x * matrixB.m_iBasis.w + m_translation.y * matrixB.m_jBasis.w + m_translation.z * matrixB.m_kBasis.w + m_translation.w * matrixB.m_translation.w;

	return product;
}


bool Matrix4::IsOrthogonal()
{
	Matrix4 transposeMatrix = *this;
	transposeMatrix.Transpose();

	Matrix4 orthogonalMatrix = Multiply(transposeMatrix);
	return orthogonalMatrix.IsIdentity();
}

bool Matrix4::IsIdentity()
{
	return *this == Matrix4::IDENTITY;
}


Matrix4 Matrix4::operator*(const float &floatA)
{
	m_iBasis.x = m_iBasis.x	* floatA;
	m_iBasis.y = m_iBasis.y	* floatA;
	m_iBasis.z = m_iBasis.z	* floatA;
	m_iBasis.w = m_iBasis.w	* floatA;

	m_jBasis.x = m_jBasis.x	* floatA;
	m_jBasis.y = m_jBasis.y	* floatA;
	m_jBasis.z = m_jBasis.z	* floatA;
	m_jBasis.w = m_jBasis.w	* floatA;

	m_kBasis.x = m_kBasis.x	* floatA;
	m_kBasis.y = m_kBasis.y	* floatA;
	m_kBasis.z = m_kBasis.z	* floatA;
	m_kBasis.w = m_kBasis.w	* floatA;

	m_translation.x = m_translation.x * floatA;
	m_translation.y = m_translation.y * floatA;
	m_translation.z = m_translation.z * floatA;
	m_translation.w = m_translation.w * floatA;

	return *this;
}

Matrix4 Matrix4::CreateTranslation(const Vector2& translation2D)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_translation.x = translation2D.x;
	translatedMatrix.m_translation.y = translation2D.y;

	return translatedMatrix;
}

Matrix4 Matrix4::CreateTranslation(const Vector3& translation3D)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_translation.x = translation3D.x;
	translatedMatrix.m_translation.y = translation3D.y;
	translatedMatrix.m_translation.z = translation3D.z;

	return translatedMatrix;
}

Matrix4 Matrix4::CreateScale(float uniformScale)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.Scale(uniformScale);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateScale(const Vector2& nonUniformScale2D)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x *= nonUniformScale2D.x;
	translatedMatrix.m_jBasis.y *= nonUniformScale2D.y;

	return translatedMatrix;
}

Matrix4 Matrix4::CreateScale(const Vector3& nonUniformScale3D)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x *= nonUniformScale3D.x;
	translatedMatrix.m_jBasis.y *= nonUniformScale3D.y;
	translatedMatrix.m_jBasis.z *= nonUniformScale3D.z;

	return translatedMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutX(float degrees)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_jBasis.y = CosDegrees(degrees);
	translatedMatrix.m_jBasis.z = SinDegrees(degrees);

	translatedMatrix.m_kBasis.y = -SinDegrees(degrees);
	translatedMatrix.m_kBasis.z = CosDegrees(degrees);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutY(float degrees)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x = CosDegrees(degrees);
	translatedMatrix.m_iBasis.z = -SinDegrees(degrees);

	translatedMatrix.m_kBasis.x = SinDegrees(degrees);
	translatedMatrix.m_kBasis.z = CosDegrees(degrees);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateRotationDegreesAboutZ(float degrees)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x = CosDegrees(degrees);
	translatedMatrix.m_iBasis.y = SinDegrees(degrees);

	translatedMatrix.m_jBasis.x = -SinDegrees(degrees);
	translatedMatrix.m_jBasis.y = CosDegrees(degrees);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateRotationRadiansAboutX(float radians)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_jBasis.y = (float) cos(radians);
	translatedMatrix.m_jBasis.z = (float) sin(radians);

	translatedMatrix.m_kBasis.y = (float) -sin(radians);
	translatedMatrix.m_kBasis.z = (float) cos(radians);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateRotationRadiansAboutY(float radians)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x = (float) cos(radians);
	translatedMatrix.m_iBasis.z = (float) -sin(radians);

	translatedMatrix.m_kBasis.x = (float) sin(radians);
	translatedMatrix.m_kBasis.z = (float) cos(radians);

	return translatedMatrix;
}

Matrix4 Matrix4::CreateFromRows(const Vector4 &row1, const Vector4 &row2, const Vector4 &row3, const Vector4 &row4)
{
	Matrix4 newMatrix;

	newMatrix.m_iBasis = Vector4( row1.x, row1.y, row1.z, row1.w );
	newMatrix.m_jBasis = Vector4( row2.x, row2.y, row2.z, row2.w );
	newMatrix.m_kBasis = Vector4( row3.x, row3.y, row3.z, row3.w );
	newMatrix.m_translation = Vector4( row4.x, row4.y, row4.z, row4.w );

	return newMatrix;
}


Matrix4 Matrix4::CreateFromBasis(const Vector3 &basis1, const Vector3 &basis2, const Vector3 &basis3)
{
	Matrix4 newMatrix;
	newMatrix.MakeIdentity();

// 	newMatrix.m_iBasis = Vector4(basis1.x, basis2.x, basis3.x, 0.f);
// 	newMatrix.m_jBasis = Vector4(basis1.y, basis2.y, basis3.y, 0.f);
// 	newMatrix.m_kBasis = Vector4(basis1.z, basis2.z, basis3.z, 0.f);

	newMatrix.m_iBasis = Vector4(basis1, 0.f);
	newMatrix.m_jBasis = Vector4(basis2, 0.f);
	newMatrix.m_kBasis = Vector4(basis3, 0.f);

	return newMatrix;
}


Matrix4 Matrix4::CreateRotationRadiansAboutZ(float radians)
{
	Matrix4 translatedMatrix;
	translatedMatrix.MakeIdentity();

	translatedMatrix.m_iBasis.x = (float) cos(radians);
	translatedMatrix.m_iBasis.y = (float) sin(radians);

	translatedMatrix.m_jBasis.x = (float) -sin(radians);
	translatedMatrix.m_jBasis.y = (float) cos(radians);

	return translatedMatrix;
}

const Matrix4 Matrix4::ZERO( Vector4(0.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 0.f, 0.f) );
const Matrix4 Matrix4::IDENTITY( Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(0.f, 0.f, 0.f, 1.f) );