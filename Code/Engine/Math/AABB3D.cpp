#include "Engine/Math/AABB3D.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtilities.hpp"

AABB3D::AABB3D()
{
	mins = Vector3(0.f, 0.f, 0.f);
	maxs = Vector3(0.f, 0.f, 0.f);
}

AABB3D::~AABB3D()
{
}

AABB3D::AABB3D(const AABB3D& copy)
{
	mins = copy.mins;
	maxs = copy.maxs;
}

AABB3D::AABB3D(float initialX, float initialY, float initialZ)
{
	mins = Vector3(initialX, initialY, initialZ);
	maxs = Vector3(initialX, initialY, initialZ);
}

AABB3D::AABB3D(float minX, float minY, float maxX, float maxY, float minZ, float maxZ)
{
	mins = Vector3(minX, minY, minZ);
	maxs = Vector3(maxX, maxY, maxZ);
}

AABB3D::AABB3D(const Vector3& copyMins, const Vector3& copyMaxs)
{
	mins = copyMins;
	maxs = copyMaxs;
}

AABB3D::AABB3D(const Vector3& center, float radiusX, float radiusY, float radiusZ)
{
	SetCenterAndRadii(center, radiusX, radiusY, radiusZ);
}

void AABB3D::SetCenterAndRadii(const Vector3& center, float radiusX, float radiusY, float radiusZ)
{
	mins.x = center.x - radiusX;
	mins.y = center.y - radiusY;
	mins.z = center.z - radiusZ;

	maxs.x = center.x + radiusX;
	maxs.y = center.y + radiusY;
	maxs.z = center.z + radiusZ;
}

void AABB3D::StretchToIncludePoint(const Vector3& point)
{
	if (point.x < mins.x)
	{
		mins.x = point.x;
	}
	if (point.y < mins.y)
	{
		mins.y = point.y;
	}
	if (point.z < mins.z)
	{
		mins.z = point.z;
	}

	if (point.x > maxs.x)
	{
		maxs.x = point.x;
	}
	if (point.y > mins.y)
	{
		maxs.y = point.y;
	}
	if (point.z > mins.z)
	{
		maxs.z = point.z;
	}
}


void AABB3D::AddPadding(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius)
{
	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;
	mins.z -= zPaddingRadius;

	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;
	mins.z += zPaddingRadius;
}

void AABB3D::Translate(const Vector3& translation)
{
	mins += translation;
	maxs += translation;
}

bool AABB3D::IsPointInside(const Vector3& point) const
{
	return (point.x > mins.x &&
		point.y > mins.y &&
		point.z > mins.z &&
		point.x < maxs.x &&
		point.y < maxs.y &&
		point.z < maxs.z);
}

const Vector3 AABB3D::CalcSize() const
{
	return maxs - mins;
}

const Vector3 AABB3D::CalcCenter() const
{
	Vector3 size = CalcSize();
	return size / 2;
}

const Vector3 AABB3D::GetPointAtNormalizedPositionWithinBox(const Vector3& normalizedPosition) const
{
	Vector2 vectorZeroToOne = Vector2(0.f, 1.f);
	Vector2 rangeX = Vector2(mins.x, maxs.x);
	Vector2 rangeY = Vector2(mins.y, maxs.y);
	Vector2 rangeZ = Vector2(mins.z, maxs.z);
	return RangeMap3D(normalizedPosition, vectorZeroToOne, vectorZeroToOne, vectorZeroToOne, rangeX, rangeY, rangeZ);
}

const Vector3 AABB3D::GetNormalizedPositionForPointWithinBox(const Vector3& point) const
{
	Vector2 vectorZeroToOne = Vector2(0.f, 1.f);
	Vector2 rangeX = Vector2(mins.x, maxs.x);
	Vector2 rangeY = Vector2(mins.y, maxs.y);
	Vector2 rangeZ = Vector2(mins.z, maxs.z);
	return RangeMap3D(point, rangeX, rangeY, rangeZ, vectorZeroToOne, vectorZeroToOne, vectorZeroToOne);
}

const AABB3D AABB3D::operator+(const Vector3& translation) const
{
	return AABB3D( mins + translation, maxs + translation);
}

const AABB3D AABB3D::operator-(const Vector3& antiTranslation) const
{
	return AABB3D(mins - antiTranslation, maxs - antiTranslation);
}

void AABB3D::operator-=(const Vector3& antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

void AABB3D::operator+=(const Vector3& translation)
{
	mins += translation;
	maxs += translation;
}

const AABB3D Interpolate(const AABB3D& start, const AABB3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	AABB3D blended;
	blended.mins = (fractionOfStart * start.mins) + (fractionToEnd * end.mins);
	blended.maxs = (fractionOfStart * start.maxs) + (fractionToEnd * end.maxs);
	return blended;
}

bool DoAABB3DsOverlap(const AABB3D& first, const AABB3D& second)
{
	if (first.mins.y > second.maxs.y ||
		second.mins.y > first.maxs.y ||
		first.mins.x > second.maxs.x ||
		second.mins.x > first.maxs.x ||
		first.mins.z > second.maxs.z ||
		second.mins.z > first.maxs.z)
		return false;
	return true;
}
