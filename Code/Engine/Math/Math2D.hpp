#pragma once
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/LineSegment2D.hpp"

class Math2D
{
public:
	Math2D();
	~Math2D();
};

bool DoAABBsOverlap(const AABB2D& first, const AABB2D& second);
bool DoDiscsOverlap(const Disc2D& first, const Disc2D& second);
bool DoesDiscOverlapAABB2D(const Disc2D& disc, const AABB2D& aabb);
bool DoesDiscOverlapLineSegment(const Disc2D& disc, const LineSegment2D& lineSegment);
float CalcAngleBetweenDiscsDegrees(const Disc2D& first, const Disc2D& second);
float CalcOverlapDistanceBetweenDiscs(const Disc2D& first, const Disc2D& second);
bool IsDiscEastOfAABB(const Disc2D& disc, const AABB2D& aabb);
bool IsDiscWestOfAABB(const Disc2D& disc, const AABB2D& aabb);
Vector2 GetRandomPointOnDiscEdge(const Disc2D& disc);

AABB2D GetAABBFromDisc(const Disc2D& disc);
