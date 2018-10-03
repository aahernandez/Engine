#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Core/RGBA.hpp"

#define D2R(deg) ((deg) * (fPI / 180.f))
const int MAX_BUILDER_UVS = 2;

class RGBA;
class Matrix4;

class Vertex
{
public:
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
	Vector2 uv2;
	Vector4 color;
	Vector3 tangent;
	Vector3 bitangent;
	UIntVector4 boneIndices;
	Vector4 boneWeights;
	unsigned int index;

	Vertex();
	Vertex(Vector3 vec);
	Vertex(Vector3 vec3, Vector2 vec2);
	Vertex(Vector3 pos, Vector3 norm, RGBA col = RGBA::WHITE);
	Vertex(Vector3 vec3, Vector2 vec2, RGBA col);
	Vertex(Vector3 pos, Vector3 norm, Vector2 tex, RGBA col);
	Vertex(Vector3 pos, Vector3 norm, Vector2 tex, Vector3 tan, RGBA col);
	~Vertex();

	void SetMeshBuilderDefault();
};
Matrix4 MatrixMakePerspectiveProjection(float const fovRadians, float const aspectRatio, float const nz, float const fz);