//Some code based off of code by Squirrel Eiserloh
#pragma once

class Vector2;
class Vector3;
class IntVector2;
class Matrix4;
class Vector4;

const float fPI = 3.1415926535897932384626433832795f;
const float fSQRT_3_OVER_3 = 0.5773502691896257645091f;

float CosDegrees(float degrees);
float SinDegrees(float degrees);

float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);
int GetRandomIntLessThan(int maxValueNotInclusive);
int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive);
int GetRandomIntInRange(IntVector2 minMaxValues);
bool GetRandomBool();
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive);

float RangeMap1D(float point, float inMin, float inMax, float outMin, float outMax);
float RangeMap1D(float point, Vector2 inRange, Vector2 outRange);
Vector2 RangeMap2D(float pointX, float pointY, float inMinX, float inMinY, float inMaxX, float inMaxY, float outMinX, float outMinY, float outMaxX, float outMaxY);
Vector2 RangeMap2D( Vector2 point, Vector2 inRangeX, Vector2 inRangeY, Vector2 outRangeX, Vector2 outRangeY);
Vector3 RangeMap3D( Vector3 point, Vector2 inRangeX, Vector2 inRangeY, Vector2 inRangeZ, Vector2 outRangeX, Vector2 outRangeY, Vector2 outRangeZ);

float FastFloor(float numToFloor);
float SmoothStep(float inputZeroToOne);
Vector3 LinearlyInterpolate(const Vector3& start, const Vector3& finish, float percentage);
Vector4 LinearlyInterpolate(const Vector4& start, const Vector4& finish, float percentage);
Matrix4 LinearlyInterpolate(const Matrix4& start, const Matrix4& finish, float percentage);
float LerpLinear(float start, float end, float percentage);
Vector4 LerpLinear(const Vector4& start, const Vector4& finish, float percentage);
Matrix4 LerpLinear(const Matrix4& start, const Matrix4& finish, float percentage);
float SmoothStep5(float inputZeroToOne);

float GetDistanceVector3(const Vector3 &start, const Vector3 &finish);

float GetMax(float num1, float num2);
float GetMin(float num1, float num2);
float Ceiling(float numToCeiling);

void ClampZeroToOne(float& numToClamp);

int CalculateManhattanDistance(IntVector2 start, IntVector2 end);

float CalculateFactorialFloat(float startingNum);

template <typename T>
T Clamp(T numToClamp, const T& minBound, const T& maxBound)
{
	if (numToClamp < minBound)
	{
		numToClamp = minBound;
	}
	else if (numToClamp > maxBound)
	{
		numToClamp = maxBound;
	}

	return numToClamp;
}