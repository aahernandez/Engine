#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere3D
{
public:
	Vector3 m_center;
	float m_radius;

	static const Sphere3D UNIT_CIRCLE;

	Sphere3D();
	~Sphere3D();
	Sphere3D(const Sphere3D& copy);
	explicit Sphere3D(float initialX, float initialY, float initialZ, float internalRadius);
	explicit Sphere3D(const Vector3& initialCenter, float initialRadius);

	bool IsPointInside(const Vector3& point) const;
	void StretchToIncludePoint(const Vector3& point);
	void AddPadding(float paddingRadius);
	void Translate(const Vector3& translation);


	const Sphere3D operator + (const Vector3& translation) const;
	const Sphere3D operator - (const Vector3& antiTranslation) const;
	void operator += (const Vector3& translation);
	void operator -= (const Vector3& antiTranslation);

	friend const Sphere3D Interpolate( const Sphere3D& start, const Sphere3D& end, float fractionToEnd );
};

bool DoSpheresOverlap(const Sphere3D& first, const Sphere3D& second );