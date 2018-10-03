#pragma once
#include "Engine/Math/Matrix4.hpp"
#include <vector>

class Pose
{
public:
	std::vector<Matrix4> localTransforms;
};