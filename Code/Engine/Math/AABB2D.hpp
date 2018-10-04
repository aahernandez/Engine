#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"

class AABB2D
{
public:
	Vector2 mins;
	Vector2 maxs;

	static const AABB2D ZERO_TO_ONE;

	AABB2D();
	~AABB2D();
	AABB2D(const AABB2D& copy);
	explicit AABB2D(float initialX, float initialY);
	explicit AABB2D(float minX, float minY, float maxX, float maxY);
	explicit AABB2D(const IntVector2& copyMins, const IntVector2& copyMaxs);
	explicit AABB2D(const Vector2& copyMins, const Vector2& copyMaxs);
	explicit AABB2D(const Vector2& center, float radiusX, float radiusY);

	void SetCenterAndRadii(const Vector2& center, float radiusX, float radiusY);
	void SetCenter(const Vector2& center);
	void SetYCenter(float centerY);
	
	void StretchToIncludePoint(const Vector2& point);
	void AddPadding(float xPaddingRadius, float yPaddingRadius);
	void Translate(const Vector2& translation);
	void RotateDegrees(float degrees);
	void SetAspectRatioPreservingWidth(float xRatio, float yRatio);
	void SetAspectRatioPreservingHeight(float xRatio, float yRatio);
	void Scale(float scale);
	void Scale(Vector2 scale);

	bool IsPointInside(const Vector2& point) const;
	const Vector2 CalcSize() const;
	const Vector2 CalcCenter() const;
	const Vector2 GetPointAtNormalizedPositionWithinBox(const Vector2& normalizedPosition) const;
	const Vector2 GetNormalizedPositionForPointWithinBox(const Vector2& point) const;

	bool operator==(const AABB2D& boundingBox) const;
	bool operator!=(const AABB2D& boundingBox) const;
	const AABB2D operator+(const Vector2& translation) const;
	const AABB2D operator-(const Vector2& antiTranslation) const;
	void operator+=(const Vector2& translation);
	void operator-=(const Vector2& antiTranslation);

	Vector2 GetPointFromNormalizedPoint(Vector2 normalizedPoint);
	Vector2 GetNormalizedPointFromPoint(Vector2 point);

	friend const AABB2D Interpolate( const AABB2D& start, const AABB2D& end, float fractionToEnd );
};

bool DoAABB2DsOverlap(const AABB2D& first, const AABB2D& second);