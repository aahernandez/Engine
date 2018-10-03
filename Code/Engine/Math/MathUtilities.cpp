//Some code based off of code by Squirrel Eiserloh
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 


float CosDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return (float) cos(radians);
}

float SinDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return (float) sin(radians);
}

float ConvertRadiansToDegrees(float radians)
{
	return radians * (180.0f / fPI);
}

float ConvertDegreesToRadians(float degrees)
{
	return degrees * (fPI / 180.0f);
}

int GetRandomIntLessThan(int maxValueNotInclusive)
{
	int randomNumber = rand();
	return randomNumber % maxValueNotInclusive;
}

int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive)
{
	maxValueInclusive += 1;
	int randomNumber = rand();
	return minValueInclusive + randomNumber % (maxValueInclusive - minValueInclusive);
}

int GetRandomIntInRange(IntVector2 minMaxValues)
{
	minMaxValues.y += 1;
	int randomNumber = rand();
	return minMaxValues.x + randomNumber % (minMaxValues.y - minMaxValues.x);
}

bool GetRandomBool()
{
	int randomInt = GetRandomIntInRange(0, 1);
	if (randomInt == 0)
		return true;
	return false;
}

float GetRandomFloatZeroToOne()
{
	int randomNumber = rand();
	return (float) randomNumber / (float) RAND_MAX;
}

float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive)
{
	float randomZeroToOne = GetRandomFloatZeroToOne();
	return minimumInclusive + ((maximumInclusive - minimumInclusive) * randomZeroToOne);
}

float RangeMap1D(float point, float inMin, float inMax, float outMin, float outMax)
{
	float mappedPoint = point;
	mappedPoint -= inMin;
	float inSize = inMax - inMin;
	if (inSize == 0)
		return point;
	mappedPoint /= inSize;
	mappedPoint *= (outMax - outMin);
	mappedPoint += outMin;
	return mappedPoint;
}

float RangeMap1D(float point, Vector2 inRange, Vector2 outRange)
{
	float mappedPoint = point;
	mappedPoint -= inRange.x;
	float inSize = inRange.y - inRange.x;
	if (inSize == 0)
		return point;
	mappedPoint /= inSize;
	mappedPoint *= (outRange.y - outRange.x);
	mappedPoint += outRange.x;
	return mappedPoint;
}

Vector2 RangeMap2D(float pointX, float pointY, float inMinX, float inMinY, float inMaxX, float inMaxY, float outMinX, float outMinY, float outMaxX, float outMaxY)
{
	float mappedPointX = RangeMap1D(pointX, inMinX, inMaxX, outMinX, outMaxX);
	float mappedPointY = RangeMap1D(pointY, inMinY, inMaxY, outMinY, outMaxY);
	return Vector2 (mappedPointX, mappedPointY);
}

Vector2 RangeMap2D( Vector2 point, Vector2 inRangeX, Vector2 inRangeY, Vector2 outRangeX, Vector2 outRangeY)
{
	Vector2 mappedPoint;
	mappedPoint.x = RangeMap1D(point.x, inRangeX, outRangeX);
	mappedPoint.y = RangeMap1D(point.y, inRangeY, outRangeY);
	return mappedPoint;
}

Vector3 RangeMap3D( Vector3 point, Vector2 inRangeX, Vector2 inRangeY, Vector2 inRangeZ, Vector2 outRangeX, Vector2 outRangeY, Vector2 outRangeZ)
{
	Vector3 mappedPoint;
	mappedPoint.x = RangeMap1D(point.x, inRangeX, outRangeX);
	mappedPoint.y = RangeMap1D(point.y, inRangeY, outRangeY);
	mappedPoint.z = RangeMap1D(point.z, inRangeZ, outRangeZ);
	return mappedPoint;
}

float FastFloor(float numToFloor)
{
	return (float) floor(numToFloor);
}

float SmoothStep(float inputZeroToOne)
{
	return (3 * (inputZeroToOne * inputZeroToOne) ) - (2 * (inputZeroToOne * inputZeroToOne * inputZeroToOne) );
}

Vector3 LinearlyInterpolate(const Vector3& start, const Vector3& finish, float percentage)
{
	Vector3 finishToStart = finish - start;

	return start + (finishToStart * percentage);
}

Matrix4 LinearlyInterpolate(const Matrix4& start, const Matrix4& finish, float percentage)
{
	Vector4 iBasis = LinearlyInterpolate(start.m_iBasis, finish.m_iBasis, percentage);
	Vector4 jBasis = LinearlyInterpolate(start.m_jBasis, finish.m_jBasis, percentage);
	Vector4 kBasis = LinearlyInterpolate(start.m_kBasis, finish.m_kBasis, percentage);
	Vector4 translation = LinearlyInterpolate(start.m_translation, finish.m_translation, percentage);
	
	return Matrix4(iBasis, jBasis, kBasis, translation);
}

Vector4 LinearlyInterpolate(const Vector4& start, const Vector4& finish, float percentage)
{
	Vector4 finishToStart = finish - start;

	return start + (finishToStart * percentage);
}

float LerpLinear(float start, float end, float percentage)
{
	return start + percentage * (end - start);
}

Vector4 LerpLinear(const Vector4& start, const Vector4& end, float percentage)
{
	Vector4 lerpedVector;
	lerpedVector.x = LerpLinear(start.x, end.x, percentage);
	lerpedVector.y = LerpLinear(start.y, end.y, percentage);
	lerpedVector.z = LerpLinear(start.z, end.z, percentage);
	lerpedVector.w = LerpLinear(start.w, end.w, percentage);
	return lerpedVector;
}

Matrix4 LerpLinear(const Matrix4& start, const Matrix4& end, float percentage)
{
	if (percentage == 1.f)
		return end;

	Matrix4 lerpedMatrix;
	lerpedMatrix.m_iBasis = LerpLinear(start.m_iBasis, end.m_iBasis, percentage);
	lerpedMatrix.m_jBasis = LerpLinear(start.m_jBasis, end.m_jBasis, percentage);
	lerpedMatrix.m_kBasis = LerpLinear(start.m_kBasis, end.m_kBasis, percentage);
	lerpedMatrix.m_translation = LerpLinear(start.m_translation, end.m_translation, percentage);
	return lerpedMatrix;
}

float SmoothStep5(float inputZeroToOne)
{
	return ( 6 * (inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne) ) - (15 * (inputZeroToOne * inputZeroToOne * inputZeroToOne * inputZeroToOne) ) + ( 10 * (inputZeroToOne * inputZeroToOne * inputZeroToOne) );
}

float GetDistanceVector3(const Vector3 &start, const Vector3 &finish)
{
	float distanceX = finish.x - start.x;
	float distanceY = finish.y - start.y;
	float distanceZ = finish.z - start.z;
	return (float)sqrt((distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ));
}

float GetMax(float num1, float num2)
{
	if (num1 > num2)
		return num1;
	return num2;
}

float GetMin(float num1, float num2)
{
	if (num1 < num2)
		return num1;
	return num2;
}

float Ceiling(float numToCeiling)
{
	return (float) ceil(numToCeiling);
}

void ClampZeroToOne(float& numToClamp)
{
	numToClamp = Clamp(numToClamp, 0.f, 1.f);
}

int CalculateManhattanDistance(IntVector2 start, IntVector2 end)
{
	return abs(start.x - end.x) + abs(start.y - end.y);
}

float CalculateFactorialFloat(float startingNum)
{
	if (startingNum == 0)
		return 1.f;
	return startingNum * CalculateFactorialFloat(startingNum - 1);
}
