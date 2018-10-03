#include "Engine/Math/Sphere3D.hpp"
#include <math.h>

Sphere3D::Sphere3D()
	: m_center(0.f, 0.f, 0.f)
	, m_radius(0.f)
{
}

Sphere3D::~Sphere3D()
{
}

Sphere3D::Sphere3D(const Sphere3D& copy)
	: m_radius(copy.m_radius)
{
	m_center = copy.m_center;
}

Sphere3D::Sphere3D(float initialX, float initialY, float initialZ, float internalRadius)
	: m_radius(internalRadius)
{
	m_center = Vector3(initialX, initialY, initialZ);
}

Sphere3D::Sphere3D(const Vector3& initialCenter, float initialRadius)
	: m_radius(initialRadius)
{
	m_center = initialCenter;
}

bool Sphere3D::IsPointInside(const Vector3& point) const
{
	float distance = CalcDistance(m_center, point);
	if (distance > m_radius)
	{
		return false;
	}
	return true;
}

void Sphere3D::StretchToIncludePoint(const Vector3& point)
{
	if (!IsPointInside(point))
	{
		m_radius = CalcDistance(m_center, point);
	}
}

void Sphere3D::AddPadding(float paddingRadius)
{
	m_radius += paddingRadius;
}

void Sphere3D::Translate(const Vector3& translation)
{
	m_center += translation;
}

const Sphere3D Sphere3D::operator+(const Vector3& translation) const
{
	return Sphere3D(m_center + translation, m_radius);
}

const Sphere3D Sphere3D::operator-(const Vector3& antiTranslation) const
{
	return Sphere3D(m_center - antiTranslation, m_radius);
}

void Sphere3D::operator+=(const Vector3& translation)
{
	m_center += translation;
}

void Sphere3D::operator-=(const Vector3& antiTranslation)
{
	m_center -= antiTranslation;
}

const Sphere3D Interpolate(const Sphere3D& start, const Sphere3D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Sphere3D blended;
	blended.m_center = (fractionOfStart * start.m_center) + (fractionToEnd * end.m_center);
	blended.m_radius = (fractionOfStart * start.m_radius) + (fractionToEnd * end.m_radius);
	return blended;
}

bool DoSpheresOverlap(const Sphere3D& first, const Sphere3D& second)
{
	float distanceX = second.m_center.x - first.m_center.x;
	float distanceY = second.m_center.y - first.m_center.y;
	float distanceZ = second.m_center.z - first.m_center.z;
	float distance = (float) sqrt( (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ) );

	if (distance > first.m_radius + second.m_radius)
	{
		return false;
	}
	return true;
}
