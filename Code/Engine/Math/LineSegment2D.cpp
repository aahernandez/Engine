#include "Engine/Math/LineSegment2D.hpp"

LineSegment2D::LineSegment2D()
	: lineStart(0.0f, 0.0f)
	, lineEnd(0.f, 0.f)
{
}

LineSegment2D::LineSegment2D(const Vector2& center, float length)
{
	lineStart = Vector2(center.x, center.y + length);
	lineEnd = Vector2(center.x, center.y - length);
}

LineSegment2D::~LineSegment2D()
{
}

LineSegment2D::LineSegment2D(const LineSegment2D& copy)
	: lineStart(copy.lineStart)
	, lineEnd(copy.lineEnd)
{
}

LineSegment2D::LineSegment2D(const Vector2& startCopy, const Vector2& endCopy)
	: lineStart(startCopy)
	, lineEnd(endCopy)
{
}

LineSegment2D::LineSegment2D(float initialStartX, float initialStartY, float initialEndX, float initialEndY)
	: lineStart(initialStartX, initialStartY)
	, lineEnd(initialEndX, initialEndY)
{
}

const Vector2 LineSegment2D::CalcCenter() const
{
	Vector2 size = lineEnd + lineStart;
	return size * 0.5f;
}


const float LineSegment2D::CalcLength() const
{
	float distanceX = lineEnd.x - lineStart.x;
	float distanceY = lineEnd.y - lineStart.y;
	return (float) sqrt((distanceX * distanceX) + (distanceY * distanceY));
}

void LineSegment2D::SetCenter(const Vector2& center)
{
	float length = CalcLength() * 0.5f;
	lineStart = Vector2(center.x, center.y + length);
	lineEnd = Vector2(center.x, center.y - length);
}

void LineSegment2D::RotateDegrees(float degreesToRotate)
{
	float cosDeg = CosDegrees(degreesToRotate);
	float sinDeg = SinDegrees(degreesToRotate);

	Vector2 midpoint = CalcCenter();

	Vector2 aMidpoint = lineStart - midpoint;
	Vector2 bMidpoint = lineEnd - midpoint;

	Vector2 aRotated;
	aRotated.x = cosDeg * aMidpoint.x - sinDeg * aMidpoint.y;
	aRotated.y = sinDeg * aMidpoint.x + cosDeg * aMidpoint.y;

	Vector2 bRotated;
	bRotated.x = cosDeg * bMidpoint.x - sinDeg * bMidpoint.y;
	bRotated.y = sinDeg * bMidpoint.x + cosDeg * bMidpoint.y;

	aRotated += midpoint;
	bRotated += midpoint;

	lineStart = aRotated;
	lineEnd = bRotated;
}

void LineSegment2D::SetRotationDegreesAroundPoint(const Vector2 &point, float degreesToRotate)
{
	float cosDeg = CosDegrees(-degreesToRotate + 90.f);
	float sinDeg = SinDegrees(-degreesToRotate + 90.f);
	float length = CalcLength();
	Vector2 center = CalcCenter();
	float centerRadius = center.CalcDistanceToVector(point);
	float firstRadius = centerRadius + (length * 0.5f);
	float secondRadius = centerRadius - (length * 0.5f);

	lineStart.x = cosDeg * firstRadius + point.x;
	lineStart.y = sinDeg * firstRadius + point.y;
	lineEnd.x = cosDeg * secondRadius + point.x;
	lineEnd.y = sinDeg * secondRadius + point.y;
}

float LineSegment2D::GetRotationDegreesFromStartToEnd() const
{
	Vector2 displacement = lineEnd - lineStart;
	Vector2 normalizedDisplacement = displacement.GetVectorNormal(displacement);
	float radians = atan2f(normalizedDisplacement.x, normalizedDisplacement.y);
	float degrees = ConvertRadiansToDegrees(radians);
	return degrees;
}

void LineSegment2D::Translate(const Vector2& translation)
{
	lineStart += translation;
	lineEnd += translation;
}

const LineSegment2D Interpolate(const LineSegment2D& start, const LineSegment2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	LineSegment2D blended;
	blended.lineStart = (fractionOfStart * start.lineStart) + (fractionToEnd * end.lineStart);
	blended.lineEnd = (fractionOfStart * start.lineEnd) + (fractionToEnd * end.lineEnd);
	return blended;
}

const LineSegment2D LineSegment2D::ZERO(Vector2::ZERO, Vector2::ZERO);