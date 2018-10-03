#pragma once
#include "Engine/Math/Plane3D.hpp"
#include <vector>

class ConvexHull3D
{
public:
	std::vector<Plane3D> m_planes;

	ConvexHull3D();
	ConvexHull3D(const std::vector<Plane3D> planes);
	~ConvexHull3D();

	void AddPlane(Plane3D);

private:

};