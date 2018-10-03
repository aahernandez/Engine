//Code based off of code by Squirrel Eiserloh
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "ThirdParty/STB/stb_image.h"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
// #include "Engine/Renderer/OpenGLExtensions.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <gl/Glu.h>
#include "ThirdParty/OpenGL/wglext.h"
#include "ThirdParty/OpenGL/glext.h"


const unsigned int PRIMITIVE_QUADS = GL_QUADS;
const unsigned int PRIMITIVE_TRIANGLES = GL_TRIANGLES;
const unsigned int PRIMITIVE_LINES = GL_LINES;
const unsigned int PRIMITIVE_LINE_LOOP = GL_LINE_LOOP;
const unsigned int PRIMITIVE_POINTS = GL_POINTS;

PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;


Renderer::Renderer()
{
	glGenBuffers	= (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glBindBuffer	= (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData	= (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");

	GUARANTEE_OR_DIE( glGenBuffers != nullptr, "ERROR: This card does not support glGenBuffers!");
	GUARANTEE_OR_DIE( glDeleteBuffers != nullptr, "ERROR: This card does not support glDeleteBuffers!");
	GUARANTEE_OR_DIE( glBindBuffer != nullptr, "ERROR: This card does not support glBindBuffer!");
	GUARANTEE_OR_DIE( glBufferData != nullptr, "ERROR: This card does not support glBufferData!");

	defaultWhiteTexture = CreateOrGetTexture("Data/Images/White.png");
}

Renderer::~Renderer()
{
}

void Renderer::SetColor(const RGBA& clearColor)
{
	glColor4ub(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}

void Renderer::ClearScreen(const RGBA& clearColor)
{
	float r, g, b, a;
	clearColor.GetAsFloats(r, g, b, a);
	glClearColor(r, g, b, a);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetOrtho(const Vector2& bottomLeft, const Vector2& topRight)
{
	glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.0f, 1.0f);
}

void Renderer::SetPerspective(float fovYDegrees, float aspectRatio, float nearClipDistance, float farClipDistance)
{
	glLoadIdentity();
	gluPerspective(fovYDegrees, aspectRatio, nearClipDistance, farClipDistance);
}

void Renderer::DrawLine3D(const Vector3& start, const Vector3& end, const RGBA& startColor, const RGBA& endColor)
{
	const int  NUM_VERTS = 2;
	Vertex3_PCT vertexes[NUM_VERTS];
	vertexes[0].m_position =  start;
	vertexes[0].m_color = startColor;
	vertexes[0].m_texCoords = Vector2(0.f, 0.f);
	vertexes[1].m_position = end;
	vertexes[1].m_color = endColor;
	vertexes[1].m_texCoords = Vector2(0.f, 0.f);

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_LINES);
}

void Renderer::DrawBoxOpen2D(const AABB2D& bounds, const RGBA& color /*= RGBA::WHITE*/)
{
	const int  NUM_VERTS = 4;
	Vertex3_PCT vertexes[NUM_VERTS];

	vertexes[0].m_position = Vector3(bounds.mins.x, bounds.mins.y, 0.f);
	vertexes[0].m_color = color;
	vertexes[0].m_texCoords = Vector2(0.f, 0.f);

	vertexes[1].m_position = Vector3(bounds.maxs.x, bounds.mins.y, 0.f);
	vertexes[1].m_color = color;
	vertexes[1].m_texCoords = Vector2(0.f, 0.f);

	vertexes[2].m_position = Vector3(bounds.maxs.x, bounds.maxs.y, 0.f);
	vertexes[2].m_color = color;
	vertexes[2].m_texCoords = Vector2(0.f, 0.f);

	vertexes[3].m_position = Vector3(bounds.mins.x, bounds.maxs.y, 0.f);
	vertexes[3].m_color = color;
	vertexes[3].m_texCoords = Vector2(0.f, 0.f);

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_LINE_LOOP);
}

void Renderer::DrawBoxOpen3D(const AABB3D& boxBounds, const RGBA& color /*= RGBA::WHITE*/)
{
	const int  NUM_VERTS = 8;
	Vertex3_PCT vertexes[NUM_VERTS];

	vertexes[0].m_position = boxBounds.mins;
	vertexes[0].m_color = color;
	vertexes[0].m_texCoords = Vector2(0.f, 0.f);

	vertexes[1].m_position = Vector3(boxBounds.maxs.x, boxBounds.mins.y, boxBounds.mins.z);
	vertexes[1].m_color = color;
	vertexes[1].m_texCoords = Vector2(0.f, 0.f);

	vertexes[2].m_position = Vector3(boxBounds.mins.x, boxBounds.maxs.y, boxBounds.mins.z);
	vertexes[2].m_color = color;
	vertexes[2].m_texCoords = Vector2(0.f, 0.f);

	vertexes[3].m_position = Vector3(boxBounds.maxs.x, boxBounds.maxs.y, boxBounds.mins.z);
	vertexes[3].m_color = color;
	vertexes[3].m_texCoords = Vector2(0.f, 0.f);


	vertexes[4].m_position = Vector3(boxBounds.mins.x, boxBounds.mins.y, boxBounds.maxs.z);
	vertexes[4].m_color = color;
	vertexes[4].m_texCoords = Vector2(0.f, 0.f);

	vertexes[5].m_position = Vector3(boxBounds.maxs.x, boxBounds.mins.y, boxBounds.maxs.z);
	vertexes[5].m_color = color;
	vertexes[5].m_texCoords = Vector2(0.f, 0.f);

	vertexes[6].m_position = Vector3(boxBounds.mins.x, boxBounds.maxs.y, boxBounds.maxs.z);
	vertexes[6].m_color = color;
	vertexes[6].m_texCoords = Vector2(0.f, 0.f);

	vertexes[7].m_position = boxBounds.maxs;
	vertexes[7].m_color = color;
	vertexes[7].m_texCoords = Vector2(0.f, 0.f);

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_LINES);
}

void Renderer::DrawDiscOpen2D(float radius, int numVertexes, const RGBA& color /*= RGBA::WHITE*/)
{
	float degreesPerVertex = (float)(360.f / numVertexes);
	std::vector< Vertex3_PCT > vertexes;
	vertexes.resize(numVertexes);

	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = radius * CosDegrees(degreesThisVertex);
		float y = radius * SinDegrees(degreesThisVertex);
		vertexes[vertexIndex].m_position = Vector3(x, y, 0.f);
		vertexes[vertexIndex].m_color = color;
		vertexes[vertexIndex].m_texCoords = Vector2(0.f, 0.f);
	}

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(&vertexes[0], numVertexes, GL_LINE_LOOP);
}

void Renderer::DrawDiscClosed2D(Vector2 pos, float radius, int numVertexes, const RGBA& color /*= RGBA::WHITE*/)
{
	float degreesPerVertex = (float)(360.f / numVertexes);
	std::vector< Vertex3_PCT > vertexes;
	vertexes.resize(numVertexes);

	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = pos.x + (radius * CosDegrees(degreesThisVertex));
		float y = pos.y + (radius * SinDegrees(degreesThisVertex));
		vertexes[vertexIndex].m_position = Vector3(x, y, 0.f);
		vertexes[vertexIndex].m_color = color;
		vertexes[vertexIndex].m_texCoords = Vector2(0.f, 0.f);
	}

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(&vertexes[0], numVertexes, GL_POLYGON);
}

void Renderer::DrawCylinderHollow3D(float radius, float height, int numVertexes, const RGBA& color)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	Translate(0.f, 0.f, height * 0.5f);
	DrawDiscOpen2D(radius, numVertexes, color);

	Translate(0.f, 0.f, -height );

	float degreesPerVertex = (float)(360.f / numVertexes);
	std::vector< Vertex3_PCT > vertexes;
	vertexes.resize(numVertexes);

	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = radius * CosDegrees(degreesThisVertex);
		float y = radius * SinDegrees(degreesThisVertex);
		Translate(x, y, 0.f);
		DrawLine3D( Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, height), color, color);
		Translate(-x, -y, 0.f);
		vertexes[vertexIndex].m_position = Vector3(x, y, 0.f);
		vertexes[vertexIndex].m_color = color;
		vertexes[vertexIndex].m_texCoords = Vector2(0.f, 0.f);
	}

	BindTexture2D(nullptr);

	DrawVertexArray3D_PCT(&vertexes[0], numVertexes, GL_LINE_LOOP);
}

void Renderer::Scale(float scaleFactor)
{
	glScalef(scaleFactor, scaleFactor, 0.f);
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::Translate(float xTranslation, float yTranslation, float zTranlation /*= 0.f*/)
{
	glTranslatef(xTranslation, yTranslation, zTranlation);
}

void Renderer::Rotate(float rotationDegrees, float rotationOnXAxis, float rotationOnYAxis, float rotationOnZAxis)
{
	glRotatef(rotationDegrees, rotationOnXAxis, rotationOnYAxis, rotationOnZAxis);
}

void Renderer::Rotate(float rotationDegrees, const Vector3& rotationAxis)
{
	glRotatef(rotationDegrees, rotationAxis.x, rotationAxis.y, rotationAxis.z);
}

void Renderer::LoadIdentity()
{
	glLoadIdentity();
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetAdditiveBlending()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

void Renderer::SetAlphaBlending()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Renderer::BindTexture2D(Texture* textureToBind)
{
	if (textureToBind != nullptr)
		glBindTexture(GL_TEXTURE_2D, textureToBind->m_textureID);
	else
		glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture->m_textureID);
}

void Renderer::SetDepthTesting(bool isDepthTesting)
{
	if (isDepthTesting)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void Renderer::SetLineWidth(float lineWidth)
{
	glLineWidth(lineWidth);
}

void Renderer::DrawAABB2DTextured(const AABB2D& bounds, Texture* texture, const RGBA& color)
{
	glEnable(GL_TEXTURE_2D);

	BindTexture2D(texture);

	const int  NUM_VERTS = 4;
	Vertex3_PCT vertexes[NUM_VERTS];
	vertexes[0].m_position = Vector3(bounds.mins.x, bounds.mins.y, 0.f);
	vertexes[0].m_color = color;
	vertexes[0].m_texCoords = Vector2(0.f, 1.f);
	vertexes[1].m_position = Vector3(bounds.maxs.x, bounds.mins.y, 0.f);
	vertexes[1].m_color = color;
	vertexes[1].m_texCoords = Vector2(1.f, 1.f);
	vertexes[2].m_position = Vector3(bounds.maxs.x, bounds.maxs.y, 0.f);
	vertexes[2].m_color = color;
	vertexes[2].m_texCoords = Vector2(1.f, 0.f);
	vertexes[3].m_position = Vector3(bounds.mins.x, bounds.maxs.y, 0.f);
	vertexes[3].m_color = color;
	vertexes[3].m_texCoords = Vector2(0.f, 0.f);

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_QUADS);
}

void Renderer::DrawQuadTextured3D(Vertex3_PCT one, Vertex3_PCT two, Vertex3_PCT three, Vertex3_PCT four, Texture* texture)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	BindTexture2D(texture);

	const int NUM_VERTS = 4;
	Vertex3_PCT vertexes[NUM_VERTS];

	vertexes[0] = one;
	vertexes[1] = two;
	vertexes[2] = three;
	vertexes[3] = four;

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_QUADS);
}

void Renderer::DrawVoxelTextured3D(Vector3 position, Texture* texture)
{
	Vertex3_PCT one =	Vertex3_PCT(Vector3(position.x			, position.y		, position.z)		, RGBA::WHITE, Vector2(1.f, 0.f));
	Vertex3_PCT two =	Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z)		, RGBA::WHITE, Vector2(1.f, 1.f));	
	Vertex3_PCT three = Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(0.f, 1.f));	
	Vertex3_PCT four =	Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(0.f, 0.f));	
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(Vector3(position.x			, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 1.f));
	two =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 0.f));		 
	three =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 0.f));		 
	four =				Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 1.f));		 
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(1.f, 1.f));
	two =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(0.f, 1.f));
	three =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 0.f));
	four =				Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 0.f));
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(Vector3(position.x			, position.y		, position.z)		, RGBA::WHITE, Vector2(0.f, 1.f));
	two =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z)		, RGBA::WHITE, Vector2(1.f, 1.f));
	three =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 0.f));
	four =				Vertex3_PCT(Vector3(position.x			, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 0.f));
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z)		, RGBA::WHITE, Vector2(0.f, 1.f));
	two =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 0.f));
	three =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 0.f));
	four =				Vertex3_PCT(Vector3(position.x + 1.f	, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(1.f, 1.f));
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(Vector3(position.x			, position.y		, position.z)		, RGBA::WHITE, Vector2(1.f, 1.f));
	two =				Vertex3_PCT(Vector3(position.x			, position.y		, position.z + 1.f)	, RGBA::WHITE, Vector2(1.f, 0.f));
	three =				Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z + 1.f)	, RGBA::WHITE, Vector2(0.f, 0.f));
	four =				Vertex3_PCT(Vector3(position.x			, position.y + 1.f	, position.z)		, RGBA::WHITE, Vector2(0.f, 1.f));
	DrawQuadTextured3D(one, two, three, four, texture);
}

void Renderer::DrawVoxelTexturedFromSpriteSheet3D(IntVector3 position, Texture* texture, const AABB2D& texBounds)
{
	Vertex3_PCT one =	Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));
	Vertex3_PCT two =	Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE,	texBounds.maxs);	
	Vertex3_PCT three = Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z),		RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));	
	Vertex3_PCT four =	Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE,	texBounds.mins);	
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE,	texBounds.maxs);
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z + 1),	RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));		 
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE,	texBounds.mins);		 
	four =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));		 
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));
	two =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE,	texBounds.mins);
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));
	four =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z),		RGBA::WHITE,	texBounds.maxs);
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE,	texBounds.maxs);
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z + 1),	RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));
	four =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE,	texBounds.mins);
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE,	texBounds.maxs);
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,		position.z),	RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE,	texBounds.mins);
	four =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,	position.z + 1),		RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE,	texBounds.maxs);
	two =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE,	Vector2(texBounds.maxs.x, texBounds.mins.y));
	three =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE,	texBounds.mins);
	four =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE,	Vector2(texBounds.mins.x, texBounds.maxs.y));
	DrawQuadTextured3D(one, two, three, four, texture);
}

void Renderer::DrawVoxelTexturedFromSpriteSheet3D(IntVector3 position, Texture* texture, const AABB2D& spriteSheetBoundsTop, const AABB2D& spriteSheetBoundsBottom, const AABB2D& spriteSheetBoundsSide)
{
	Vertex3_PCT one =	Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE, Vector2(spriteSheetBoundsBottom.maxs.x, spriteSheetBoundsBottom.mins.y));
	Vertex3_PCT two =	Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE, spriteSheetBoundsBottom.maxs);	
	Vertex3_PCT three = Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z),		RGBA::WHITE, Vector2(spriteSheetBoundsBottom.mins.x, spriteSheetBoundsBottom.maxs.y));	
	Vertex3_PCT four =	Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE, spriteSheetBoundsBottom.mins);	
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE, spriteSheetBoundsTop.maxs);
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z + 1),	RGBA::WHITE, Vector2(spriteSheetBoundsTop.maxs.x, spriteSheetBoundsTop.mins.y));		 
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE, spriteSheetBoundsTop.mins);		 
	four =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE, Vector2(spriteSheetBoundsTop.mins.x, spriteSheetBoundsTop.maxs.y));		 
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE, Vector2(spriteSheetBoundsSide.mins.x, spriteSheetBoundsSide.maxs.y));
	two =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE, spriteSheetBoundsSide.mins);
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE, Vector2(spriteSheetBoundsSide.maxs.x, spriteSheetBoundsSide.mins.y));
	four =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z),		RGBA::WHITE, spriteSheetBoundsSide.maxs);
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE, Vector2(spriteSheetBoundsSide.mins.x, spriteSheetBoundsSide.maxs.y));
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE, spriteSheetBoundsSide.maxs);
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z + 1),	RGBA::WHITE, Vector2(spriteSheetBoundsSide.maxs.x, spriteSheetBoundsSide.mins.y));
	four =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE, spriteSheetBoundsSide.mins);
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,		position.z),		RGBA::WHITE, spriteSheetBoundsSide.maxs);
	two =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,		position.z),	RGBA::WHITE, Vector2(spriteSheetBoundsSide.mins.x, spriteSheetBoundsSide.maxs.y));
	three =				Vertex3_PCT(IntVector3(position.x + 1,	position.y + 1,	position.z + 1),	RGBA::WHITE, spriteSheetBoundsSide.mins);
	four =				Vertex3_PCT(IntVector3(position.x + 1,	position.y,	position.z + 1),		RGBA::WHITE, Vector2(spriteSheetBoundsSide.maxs.x, spriteSheetBoundsSide.mins.y));
	DrawQuadTextured3D(one, two, three, four, texture);

	one =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z),		RGBA::WHITE, spriteSheetBoundsSide.maxs);
	two =				Vertex3_PCT(IntVector3(position.x,		position.y,		position.z + 1),	RGBA::WHITE, Vector2(spriteSheetBoundsSide.maxs.x, spriteSheetBoundsSide.mins.y));
	three =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z + 1),	RGBA::WHITE, spriteSheetBoundsSide.mins);
	four =				Vertex3_PCT(IntVector3(position.x,		position.y + 1,	position.z),		RGBA::WHITE, Vector2(spriteSheetBoundsSide.mins.x, spriteSheetBoundsSide.maxs.y));
	DrawQuadTextured3D(one, two, three, four, texture);
}

void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const RGBA& tint /*= RGBA::WHITE*/, float cellAspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr*/)
{
	cellHeight;
	glDisable(GL_DEPTH_TEST);
	SetColor(tint);
	for (int asciiTextIndex = 0; asciiTextIndex < (int) asciiText.length(); ++asciiTextIndex)
	{
		PushMatrix();
		Translate(startBottomLeft.x, startBottomLeft.y);
		Scale(cellAspectScale);
		Translate((float) asciiTextIndex, 0.f);
		DrawAABB2DTexturedFromSpriteSheet( AABB2D(0.f, 0.f, 1.f, 1.f), font->GetTexCoordsForGlyph(asciiText[asciiTextIndex]), font->m_spriteSheet->GetSpriteSheetTexture() );
		PopMatrix();
	}
}

void Renderer::DrawVertexArray3D_PCT(const Vertex3_PCT* vertexArray, int numVertexes, unsigned int primitiveDrawMode)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3_PCT), &vertexArray[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3_PCT), &vertexArray[0].m_color);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3_PCT), &vertexArray[0].m_texCoords);

	glDrawArrays(primitiveDrawMode, 0, numVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void Renderer::DrawVBO3D_PCT(unsigned int vboID, int numVertexes, unsigned int primitiveDrawMode)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(	3, GL_FLOAT,			sizeof(Vertex3_PCT), (const GLvoid*) offsetof( Vertex3_PCT, m_position ) );
	glColorPointer(		4, GL_UNSIGNED_BYTE,	sizeof(Vertex3_PCT), (const GLvoid*) offsetof( Vertex3_PCT, m_color ) );
	glTexCoordPointer(	2, GL_FLOAT,			sizeof(Vertex3_PCT), (const GLvoid*) offsetof( Vertex3_PCT, m_texCoords ) );

	glDrawArrays(primitiveDrawMode, 0, numVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_TEXTURE_2D);
}

void Renderer::UpdateVBO(unsigned int vboID, Vertex3_PCT* vertexArray, int numVertexes)
{
	size_t vertexArrayNumBytes = numVertexes * sizeof(Vertex3_PCT);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexArrayNumBytes, vertexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int Renderer::CreateVBO()
{
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	return vboID;
}

void Renderer::DestroyVBO(unsigned int vboID)
{
	glDeleteBuffers(1, &vboID);
}

void Renderer::DrawAABB2DTexturedFromSpriteSheet(const AABB2D& bounds, const AABB2D& spriteOnSheetBounds, Texture* texture)
{
	BindTexture2D(texture);
	RGBA color = RGBA::WHITE;

	const int NUM_VERTS = 4;
	Vertex3_PCT vertexes[NUM_VERTS];

	vertexes[0].m_position = Vector3(bounds.mins.x, bounds.mins.y, 0.f);
	vertexes[0].m_color = color;
	vertexes[0].m_texCoords = Vector2(spriteOnSheetBounds.mins.x, spriteOnSheetBounds.maxs.y);

	vertexes[1].m_position = Vector3(bounds.maxs.x, bounds.mins.y, 0.f);
	vertexes[1].m_color = color;
	vertexes[1].m_texCoords = Vector2(spriteOnSheetBounds.maxs.x, spriteOnSheetBounds.maxs.y);

	vertexes[2].m_position = Vector3(bounds.maxs.x, bounds.maxs.y, 0.f);
	vertexes[2].m_color = color;
	vertexes[2].m_texCoords = Vector2(spriteOnSheetBounds.maxs.x, spriteOnSheetBounds.mins.y);

	vertexes[3].m_position = Vector3(bounds.mins.x, bounds.maxs.y, 0.f);
	vertexes[3].m_color = color;
	vertexes[3].m_texCoords = Vector2(spriteOnSheetBounds.mins.x, spriteOnSheetBounds.mins.y);

	DrawVertexArray3D_PCT(vertexes, NUM_VERTS, PRIMITIVE_QUADS);
}

Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	Texture* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTexture(imageFilePath);
	return texture;
}

Texture* Renderer::CreateTexture(const std::string& imageFilePath)
{
	// Load image data
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = imageFilePath;
	texture->m_texelDimensions.SetXY(width, height);

	m_loadedTextures.push_back(texture);
	return texture;
}

unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	// Create a texture ID (or "name" as OpenGL calls it) for this new texture
	unsigned int openGLTextureID = 0xFFFFFFFF;
	glGenTextures(1, &openGLTextureID);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture(GL_TEXTURE_2D, openGLTextureID);

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

																	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (bytesPerTexel == 3)
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageTexelBytes);	// Location of the actual pixel data bytes/buffer

	return openGLTextureID;
}

Texture* Renderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)m_loadedTextures.size(); ++textureIndex)
	{
		Texture* texture = m_loadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}

	return nullptr;
}

BitmapFont* Renderer::CreateOrGetFont(const std::string& bitmapFontName)
{
	BitmapFont* bitmapFont = GetBitmapFont(bitmapFontName);

	if (bitmapFont)
		return bitmapFont;

	bitmapFont = CreateBitmapFont(bitmapFontName);
	return bitmapFont;
}

BitmapFont* Renderer::GetBitmapFont(const std::string& bitmapFontName)
{
	if ( m_loadedFonts.count(bitmapFontName) == 1 )
	{
		return m_loadedFonts[bitmapFontName];
	}

	return nullptr;
}

BitmapFont* Renderer::CreateBitmapFont(const std::string& bitmapFontName)
{
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	std::string bitmapFontFileName = "Data/Fonts/" + bitmapFontName + '.' + "png";
	unsigned char* imageTexelBytes = stbi_load(bitmapFontFileName.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", bitmapFontFileName.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", bitmapFontFileName.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = bitmapFontFileName;
	texture->m_texelDimensions.SetXY(width, height);

	SpriteSheet* spriteSheet = new SpriteSheet(texture, 16, 16);

	// Create (new) a Texture object
	BitmapFont* bitmapFont = new BitmapFont();
	bitmapFont->m_spriteSheet = spriteSheet;

	m_loadedFonts[bitmapFontName] = bitmapFont;
	return bitmapFont;
}