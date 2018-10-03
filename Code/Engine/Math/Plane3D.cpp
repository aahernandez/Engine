#include "Engine/Math/Plane3D.hpp"

Plane3D::Plane3D()
	: m_distance(0)
{
	m_displacement = Vector3(0.f, 0.f, 0.f);
}

Plane3D::Plane3D(Vector3 displacement, float distance)
{
	m_displacement = displacement;
	m_distance = distance;
}

Plane3D::~Plane3D()
{
}

bool Plane3D::IsPointOnPlane(const Vector3& point)
{
	return GetDistanceToPoint(point) == m_distance;
}

bool Plane3D::IsPointInFrontOfPlane(const Vector3& point)
{
	return GetDistanceToPoint(point) > m_distance;
}

bool Plane3D::IsPointBehindPlane(const Vector3& point)
{
	return GetDistanceToPoint(point) < m_distance;
}

float Plane3D::GetDistanceToPoint(const Vector3& point)
{
	return -DotProduct(point, m_displacement);
}

