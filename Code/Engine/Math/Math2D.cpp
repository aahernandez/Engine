#include "Engine/Math/Math2D.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"

Math2D::Math2D()
{
}

Math2D::~Math2D()
{
}

bool DoAABBsOverlap(const AABB2D& first, const AABB2D& second)
{
	if (first.mins.y > second.maxs.y ||
		second.mins.y > first.maxs.y ||
		first.mins.x > second.maxs.x ||
		second.mins.x > first.maxs.x)
		return false;
	return true;
}

bool DoDiscsOverlap(const Disc2D& first, const Disc2D& second)
{
	if (&first != nullptr && &second != nullptr)
	{
		float distanceX = second.m_center.x - first.m_center.x;
		float distanceY = second.m_center.y - first.m_center.y;
		float distance = (float)sqrt((distanceX * distanceX) + (distanceY * distanceY));

		if (distance > first.m_radius + second.m_radius)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool DoesDiscOverlapAABB2D(const Disc2D& disc, const AABB2D& aabb)
{
	return (disc.m_center.x + disc.m_radius > aabb.mins.x &&
			disc.m_center.y + disc.m_radius > aabb.mins.y &&
			disc.m_center.x - disc.m_radius < aabb.maxs.x &&
			disc.m_center.y - disc.m_radius < aabb.maxs.y);
}

bool DoesDiscOverlapLineSegment(const Disc2D& disc, const LineSegment2D& lineSegment)
{
	Vector2 discCenter = disc.m_center;
	Vector2 line = lineSegment.lineEnd - lineSegment.lineStart;
	Vector2 toStartDisplacement = lineSegment.lineEnd - discCenter;

	Vector2 normalizedLine = line / lineSegment.CalcLength();

	float dotProduct = DotProduct(toStartDisplacement, normalizedLine);

	if (dotProduct < 0)
		dotProduct = 0;
	else if (dotProduct > lineSegment.CalcLength())
		dotProduct = lineSegment.CalcLength();

	Vector2 closestPoint = lineSegment.lineEnd - (normalizedLine * dotProduct);
	return disc.IsPointInside(closestPoint);
}

float CalcAngleBetweenDiscsDegrees(const Disc2D& first, const Disc2D& second)
{
	float angle = (float) atan2(first.m_center.x - second.m_center.x, first.m_center.y - second.m_center.y);
	angle = ConvertRadiansToDegrees(angle);
	if (angle < 0.f)
		angle += 360.f;
	return angle;
}

float CalcOverlapDistanceBetweenDiscs(const Disc2D& first, const Disc2D& second)
{
	float distanceX = second.m_center.x - first.m_center.x;
	float distanceY = second.m_center.y - first.m_center.y;
	float distance = (float)sqrt((distanceX * distanceX) + (distanceY * distanceY));
	return (first.m_radius - second.m_radius) - distance;
}

bool IsDiscEastOfAABB(const Disc2D& disc, const AABB2D& aabb)
{
	if (disc.m_center.x + disc.m_radius > aabb.mins.x)
		return true;
	return false;
}

bool IsDiscWestOfAABB(const Disc2D& disc, const AABB2D& aabb)
{
	if (disc.m_center.x - disc.m_radius < aabb.maxs.x)
		return true;
	return false;
}

Vector2 GetRandomPointOnDiscEdge(const Disc2D& disc)
{
	float theta = GetRandomFloatInRange(0.f, 359.f);
	Vector2 newVector;
	newVector.x = disc.m_center.x + disc.m_radius * CosDegrees(theta);
	newVector.y = disc.m_center.y + disc.m_radius * SinDegrees(theta);
	
	return newVector;
}

AABB2D GetAABBFromDisc(const Disc2D& disc)
{
	return AABB2D(
		disc.m_center.x - disc.m_radius,
		disc.m_center.y - disc.m_radius,
		disc.m_center.x + disc.m_radius,
		disc.m_center.y + disc.m_radius);
}

