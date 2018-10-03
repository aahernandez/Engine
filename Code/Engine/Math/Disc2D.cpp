#include "Engine/Math/Disc2D.hpp"

Disc2D::Disc2D()
	: m_center(0.0f, 0.0f)
	, m_radius(0.0f)
{
}

Disc2D::~Disc2D()
{
}

Disc2D::Disc2D(const Disc2D& copy)
	: m_center(copy.m_center), m_radius(copy.m_radius)
{
}

Disc2D::Disc2D(float initialX, float initialY, float internalRadius)
	: m_center(initialX, initialY), m_radius(internalRadius)
{
}

Disc2D::Disc2D(const Vector2& initialCenter, float initialRadius)
	: m_center(initialCenter), m_radius(initialRadius)
{
}

void Disc2D::StretchToIncludePoint(const Vector2& point)
{
	if (!IsPointInside(point))
	{
		m_radius = CalcDistance(m_center, point);
	}
}

void Disc2D::AddPadding(float paddingRadius)
{
	m_radius += paddingRadius;
}

void Disc2D::Translate(const Vector2& translation)
{
	m_center += translation;
}

bool Disc2D::IsPointInside(const Vector2& point) const
{
	float distance = CalcDistance(m_center, point);
	if (distance > m_radius)
	{
		return false;
	}
	return true;
}

const Disc2D Disc2D::operator+(const Vector2& translation) const
{
	return Disc2D(m_center + translation, m_radius);
}

const Disc2D Disc2D::operator-(const Vector2& antiTranslation) const
{
	return Disc2D(m_center - antiTranslation, m_radius);
}

void Disc2D::operator+=(const Vector2& translation)
{
	m_center += translation;
}

void Disc2D::operator-=(const Vector2& antiTranslation)
{
	m_center -= antiTranslation;
}

const Disc2D Interpolate(const Disc2D& start, const Disc2D& end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	Disc2D blended;
	blended.m_center = (fractionOfStart * start.m_center) + (fractionToEnd * end.m_center);
	blended.m_radius = (fractionOfStart * start.m_radius) + (fractionToEnd * end.m_radius);
	return blended;
}
