#include "Engine/Math/LineSegment3D.hpp"
#include <math.h>

LineSegment3D::LineSegment3D()
	: lineStart(0.f, 0.f, 0.f)
	, lineEnd(0.f, 0.f, 0.f)
{
}

LineSegment3D::~LineSegment3D()
{
}

LineSegment3D::LineSegment3D(const LineSegment3D& copy)
	: lineStart(copy.lineStart)
	, lineEnd(copy.lineEnd)
{
}

LineSegment3D::LineSegment3D(const Vector3& startCopy, const Vector3& endCopy)
	: lineStart(startCopy)
	, lineEnd(endCopy)
{
}

LineSegment3D::LineSegment3D(float initialStartX, float initialStartY, float initialStartZ, float initialEndX, float initialEndY, float initialEndZ)
	: lineStart(initialStartX, initialStartY, initialStartZ)
	, lineEnd(initialEndX, initialEndY, initialEndZ)
{
}

const Vector3 LineSegment3D::CalcCenter() const
{
	Vector3 size = lineEnd - lineStart;
	return size / 2.f;
}


const float LineSegment3D::CalcLength() const
{
	float distanceX = lineEnd.x - lineStart.x;
	float distanceY = lineEnd.y - lineStart.y;
	float distanceZ = lineEnd.z - lineStart.z;
	return (float) sqrt( (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ) );
}

void LineSegment3D::Translate(const Vector3& translation)
{
	lineStart += translation;
	lineEnd += translation;
}

const LineSegment3D Interpolate(const LineSegment3D& start, const LineSegment3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	LineSegment3D blended;
	blended.lineStart = (fractionOfStart * start.lineStart) + (fractionToEnd * end.lineStart);
	blended.lineEnd = (fractionOfStart * start.lineEnd) + (fractionToEnd * end.lineEnd);
	return blended;
}