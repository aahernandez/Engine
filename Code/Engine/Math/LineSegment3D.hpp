#pragma once
#include "Engine/Math/Vector3.hpp"

class LineSegment3D
{
public:
	Vector3 lineStart;
	Vector3 lineEnd;

	LineSegment3D();
	~LineSegment3D();
	LineSegment3D(const LineSegment3D& copy);
	LineSegment3D(const Vector3& startCopy, const Vector3& endCopy);
	LineSegment3D(float initialStartX, float initialStartY, float initialStartZ, float initialEndX, float initialEndY, float initialEndZ);

	const Vector3 CalcCenter() const;
	const float CalcLength() const;

	void Translate(const Vector3& translation);

	friend const LineSegment3D Interpolate( const LineSegment3D& start, const LineSegment3D& end, float fractionToEnd );
};