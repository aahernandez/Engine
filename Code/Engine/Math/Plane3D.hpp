#pragma once
#include "Engine/Math/Vector3.hpp"

class Plane3D
{
public:
	Vector3 m_displacement;
	float m_distance;

	Plane3D();
	Plane3D(Vector3 displacement, float distance);
	~Plane3D();

	bool IsPointOnPlane(const Vector3& point);
	bool IsPointInFrontOfPlane(const Vector3& point);
	bool IsPointBehindPlane(const Vector3& point);
	float GetDistanceToPoint(const Vector3& point);
};