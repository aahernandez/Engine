#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"

class RaycastResults2D
{
public:
	bool didImpact;
	Vector2 impactPosition;
	IntVector2 impactTileCoords;
	float impactFraction;
	Vector2 impactSurfaceNormal;

	RaycastResults2D();
	~RaycastResults2D();
};