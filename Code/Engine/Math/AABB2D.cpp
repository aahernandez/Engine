#include "Engine/Math/AABB2D.hpp"

AABB2D::AABB2D()
	: mins(0.0f, 0.0f)
	, maxs(0.0f, 0.0f)
{
}

AABB2D::~AABB2D()
{
}

AABB2D::AABB2D(const AABB2D& copy)
	: mins(copy.mins)
	, maxs(copy.maxs)
{
}

AABB2D::AABB2D(float initialX, float initialY)
	: mins(initialX, initialY)
	, maxs(initialX, initialY)
{
}

AABB2D::AABB2D(float minX, float minY, float maxX, float maxY)
	: mins(minX, minY)
	, maxs(maxX, maxY)
{
}

AABB2D::AABB2D(const IntVector2& copyMins, const IntVector2& copyMaxs)
	: mins( (float) copyMins.x, (float) copyMins.y )
	, maxs( (float) copyMaxs.x, (float) copyMaxs.y )
{
}

AABB2D::AABB2D(const Vector2& copyMins, const Vector2& copyMaxs)
	: mins(copyMins)
	, maxs(copyMaxs)
{
}

AABB2D::AABB2D(const Vector2& center, float radiusX, float radiusY)
{
	SetCenterAndRadii(center, radiusX, radiusY);
}

void AABB2D::SetCenterAndRadii(const Vector2& center, float radiusX, float radiusY)
{
	mins.x = center.x - radiusX;
	mins.y = center.y - radiusY;
	maxs.x = center.x + radiusX;
	maxs.y = center.y + radiusY;
}

void AABB2D::SetCenter(const Vector2& center)
{
	Vector2 currentRadii = CalcSize() * 0.5f;
	SetCenterAndRadii(center, currentRadii.x, currentRadii.y);
}

void AABB2D::SetYCenter(float centerY)
{
	Vector2 currentCenter = CalcCenter();
	Vector2 currentRadii = CalcSize() * 0.5f;

	Vector2 newCenter(currentCenter.x, centerY);
	SetCenterAndRadii(newCenter, currentRadii.x, currentRadii.y);
}

void AABB2D::StretchToIncludePoint(const Vector2& point)
{
	if (point.x < mins.x)
	{
		mins.x = point.x;
	}
	if (point.y < mins.y)
	{
		mins.y = point.y;
	}
	if (point.x > maxs.x)
	{
		maxs.x = point.x;
	}
	if (point.y > mins.y)
	{
		maxs.y = point.y;
	}
}

void AABB2D::AddPadding(float xPaddingRadius, float yPaddingRadius)
{
	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;
	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;
}

void AABB2D::Translate(const Vector2& translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2D::RotateDegrees(float degrees)
{	
	float cosDeg = CosDegrees(degrees);
	float sinDeg = SinDegrees(degrees);
	Vector2 center = CalcCenter();
	Vector2 size = CalcSize() * 0.5f;
// 	float radius = size.CalcLength();

	

	Vector2 newPoint;
	newPoint.x = cosDeg * (mins.x - center.x) - sinDeg * (mins.y - center.y) + center.x;
	newPoint.y = sinDeg * (mins.x - center.x) + cosDeg * (mins.y - center.y) + center.y;
	mins = newPoint;

	newPoint.x = cosDeg * (maxs.x - center.x) - sinDeg * (maxs.y - center.y) + center.x;
	newPoint.y = sinDeg * (maxs.x - center.x) + cosDeg * (maxs.y - center.y) + center.y;
	maxs = newPoint;
}

void AABB2D::SetAspectRatioPreservingWidth(float xRatio, float yRatio)
{
	Vector2 size = CalcSize();
	float newYSize = (size.x / xRatio) * yRatio;
	Vector2 center = CalcCenter();
	SetCenterAndRadii(center, size.x * 0.5f, newYSize * 0.5f);
}

void AABB2D::SetAspectRatioPreservingHeight(float xRatio, float yRatio)
{
	Vector2 size = CalcSize();
	float newXSize = (size.y / yRatio) * xRatio;
	Vector2 center = CalcCenter();
	SetCenterAndRadii(center, newXSize * 0.5f, size.y * 0.5f);
}

void AABB2D::Scale(float scale)
{
	Vector2 size = CalcSize();
	Vector2 newSize = size * scale;
	Vector2 center = CalcCenter();
	SetCenterAndRadii(center, newSize.x / 2.f, newSize.y / 2.f);
}

void AABB2D::Scale(Vector2 scale)
{
	Vector2 size = CalcSize();
	Vector2 newSize = size * scale;
	Vector2 center = CalcCenter();
	SetCenterAndRadii(center, newSize.x / 2.f, newSize.y / 2.f);
}

bool AABB2D::IsPointInside(const Vector2& point) const
{
	return (point.x > mins.x &&
			point.y > mins.y &&
			point.x < maxs.x &&
			point.y < maxs.y);
}

const Vector2 AABB2D::CalcSize() const
{
	return maxs - mins;
}

const Vector2 AABB2D::CalcCenter() const
{
	Vector2 size = CalcSize();
	size *= 0.5f;
	return size + mins;
}

const Vector2 AABB2D::GetPointAtNormalizedPositionWithinBox(const Vector2& normalizedPosition) const
{
	return RangeMap2D(normalizedPosition.x, normalizedPosition.y, 0.f, 0.f, 1.f, 1.f, mins.x, mins.y, maxs.x, maxs.y);
}

const Vector2 AABB2D::GetNormalizedPositionForPointWithinBox(const Vector2& point) const
{
	return RangeMap2D(point.x, point.y, mins.x, mins.y, maxs.x, maxs.y, 0.f, 0.f, 1.f, 1.f);
}

bool AABB2D::operator==(const AABB2D& boundingBox) const
{
	return mins == boundingBox.mins && maxs == boundingBox.maxs;
}

bool AABB2D::operator!=(const AABB2D& boundingBox) const
{
	return !(*this == boundingBox);
}

const AABB2D AABB2D::operator+(const Vector2& translation) const
{
	return AABB2D(Vector2(translation.x + mins.x, translation.y + mins.y), Vector2(translation.x + maxs.x, translation.y + maxs.y));
}

const AABB2D AABB2D::operator-(const Vector2& antiTranslation) const
{
	return AABB2D(mins - antiTranslation, maxs - antiTranslation);
}

Vector2 AABB2D::GetPointFromNormalizedPoint(Vector2 normalizedPoint)
{
	return RangeMap2D(normalizedPoint, Vector2::ZERO, Vector2::ONE, Vector2(mins.x, maxs.x), Vector2(mins.y, maxs.y));
}

Vector2 AABB2D::GetNormalizedPointFromPoint(Vector2 point)
{
	return RangeMap2D(point, Vector2(mins.x, maxs.x), Vector2(mins.y, maxs.y), Vector2::ZERO, Vector2::ONE);
}

void AABB2D::operator+=(const Vector2& translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2D::operator-=(const Vector2& antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

const AABB2D Interpolate(const AABB2D& start, const AABB2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB2D blended;
	blended.mins = (fractionOfStart * start.mins) + (fractionToEnd * end.mins);
	blended.maxs = (fractionOfStart * start.maxs) + (fractionToEnd * end.maxs);
	return blended;
}

bool DoAABB2DsOverlap(const AABB2D& first, const AABB2D& second)
{
	if (first.mins.y > second.maxs.y ||
		second.mins.y > first.maxs.y ||
		first.mins.x > second.maxs.x ||
		second.mins.x > first.maxs.x)
		return false;
	return true;
}

const AABB2D AABB2D::ZERO_TO_ONE(Vector2::ZERO, Vector2::ONE);