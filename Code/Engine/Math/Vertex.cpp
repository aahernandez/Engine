#include "Engine/Math/Vertex.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Matrix4.hpp"

Vertex::Vertex()
{

}

Vertex::Vertex(Vector3 vec)
	: position(vec)
{

}

Vertex::Vertex(Vector3 vec3, Vector2 vec2)
	: position(vec3)
	, uv(vec2)
{

}

Vertex::Vertex(Vector3 vec3, Vector2 vec2, RGBA col)
	: position(vec3)
	, uv(vec2)
{
	float r, g, b, a;
	col.GetAsFloats(r, g, b, a);
	color = Vector4(r, g, b, a);

}

Vertex::Vertex(Vector3 pos, Vector3 norm, Vector2 tex, RGBA col)
	: position(pos)
	, normal(norm)
	, uv(tex)
{
	tangent = normal.Cross(Vector3(uv.x, uv.y, 0.f));// Vector3(0.f, 0.f, 0.f);

	float r, g, b, a;
	col.GetAsFloats(r, g, b, a);
	color = Vector4(r, g, b, a);
}

Vertex::Vertex(Vector3 pos, Vector3 norm, Vector2 tex, Vector3 tan, RGBA col)
	: position(pos)
	, normal(norm)
	, uv(tex)
	, tangent(tan)
{
	float r, g, b, a;
	col.GetAsFloats(r, g, b, a);
	color = Vector4(r, g, b, a);
}

Vertex::Vertex(Vector3 pos, Vector3 norm, RGBA col /*= RGBA::WHITE*/)
	: position(pos)
	, normal(norm)
{
	float r, g, b, a;
	col.GetAsFloats(r, g, b, a);
	color = Vector4(r, g, b, a);
}

Vertex::~Vertex()
{

}

void Vertex::SetMeshBuilderDefault()
{
	position = Vector3::ZERO;
	tangent = Vector3::RIGHT;
	bitangent = Vector3::UP;
	normal = -Vector3::FORWARD;
	uv = Vector2::ZERO;
	uv2 = Vector2::ZERO;
	color = RGBA::WHITE;
}

Matrix4 MatrixMakePerspectiveProjection(float const fovRadians, float const aspectRatio, float const nz, float const fz)
{
	float size = 1.f / tanf(fovRadians / 2.f);

	float w = size;
	float h = size;
	if (aspectRatio > 1.f) 
	{
		w /= aspectRatio;
	}
	else
	{
		h *= aspectRatio;
	}

	float q = 1.f / (fz - nz);

	Matrix4 mat;
	mat.m_iBasis =		Vector4(w,		0.f,	0.f,			0.f);
	mat.m_jBasis =		Vector4(0.f,	h,		0.f,			0.f);
	mat.m_kBasis =		Vector4(0.f,	0.f,	fz * q,			1.f);
	mat.m_translation = Vector4(0.f,	0.f,	-nz * fz * q,	0.f);

	return mat;
}
