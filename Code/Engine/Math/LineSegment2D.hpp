#pragma once
#include "Engine/Math/Vector2.hpp"

class LineSegment2D
{
public:
	Vector2 lineStart;
	Vector2 lineEnd;

	LineSegment2D();
	~LineSegment2D();
	LineSegment2D(const LineSegment2D& copy);
	LineSegment2D(const Vector2& center, float length);
	LineSegment2D(const Vector2& startCopy, const Vector2& endCopy);
	LineSegment2D(float initialStartX, float initialStartY, float initialEndX, float initialEndY);

	const Vector2 CalcCenter() const;
	const float CalcLength() const;

	void SetCenter(const Vector2& center);
	void RotateDegrees(float degreesToRotate);
	void SetRotationDegreesAroundPoint(const Vector2 &point, float degreesToRotate);
	float GetRotationDegreesFromStartToEnd() const;
	void Translate(const Vector2& translation);

	friend const LineSegment2D Interpolate( const LineSegment2D& start, const LineSegment2D& end, float fractionToEnd );

	static const LineSegment2D ZERO;
};