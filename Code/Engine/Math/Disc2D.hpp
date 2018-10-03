#pragma once
#include "Engine/Math/Vector2.hpp"

class Disc2D
{
public:
	Vector2 m_center;
	float m_radius;

	static const Disc2D UNIT_CIRCLE;

	Disc2D();
	~Disc2D();
	Disc2D(const Disc2D& copy);
	explicit Disc2D(float initialX, float initialY, float internalRadius);
	explicit Disc2D(const Vector2& initialCenter, float initialRadius);

	void StretchToIncludePoint(const Vector2& point);
	void AddPadding(float paddingRadius);
	void Translate(const Vector2& translation);

	bool IsPointInside(const Vector2& point) const;
	
	const Disc2D operator + (const Vector2& translation) const;
	const Disc2D operator - (const Vector2& antiTranslation) const;
	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);

	friend const Disc2D Interpolate( const Disc2D& start, const Disc2D& end, float fractionToEnd );
};