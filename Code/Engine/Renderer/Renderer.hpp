#pragma comment(lib, "opengl32")
#pragma comment(lib, "Glu32")
#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/AABB3D.hpp"
#include <vector>
#include <map>

class Texture;
class BitmapFont;

extern const unsigned int PRIMITIVE_QUADS;
extern const unsigned int PRIMITIVE_TRIANGLES;
extern const unsigned int PRIMITIVE_LINES;
extern const unsigned int PRIMITIVE_POINTS;

struct Vertex3_PCT
{
	Vector3 m_position;
	RGBA m_color;
	Vector2 m_texCoords;

	Vertex3_PCT() {};
	Vertex3_PCT(const Vector3& position, const RGBA& color, const Vector2& texCoords)
		: m_position(position)
		, m_color(color)
		, m_texCoords(texCoords)
	{
	};
	Vertex3_PCT(const IntVector3& position, const RGBA& color, const Vector2& texCoords)
		: m_position( (float) position.x, (float) position.y, (float) position.z )
		, m_color(color)
		, m_texCoords(texCoords)
	{
	};
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void DrawLine3D(const Vector3& start, const Vector3& end, const RGBA& startColor = RGBA::WHITE, const RGBA& endColor = RGBA::WHITE);
	void DrawDiscOpen2D(float radius, int numVertexes, const RGBA& color = RGBA::WHITE);
	void DrawDiscClosed2D(Vector2 pos, float radius, int numVertexes, const RGBA& color = RGBA::WHITE);
	void DrawBoxOpen2D(const AABB2D& bounds, const RGBA& color = RGBA::WHITE);
	void DrawBoxOpen3D(const AABB3D& boxBounds, const RGBA& color = RGBA::WHITE);
	void DrawCylinderHollow3D(float radius, float height, int numVertexes, const RGBA& color = RGBA::WHITE); //assumes the world has been translated to the center of the cylinder
	void DrawAABB2DTextured(const AABB2D& bounds, Texture* texture, const RGBA& color);
	void DrawAABB2DTexturedFromSpriteSheet(const AABB2D& bounds, const AABB2D& spriteOnSheetBounds, Texture* texture);
	void DrawQuadTextured3D(Vertex3_PCT one, Vertex3_PCT two, Vertex3_PCT three, Vertex3_PCT four, Texture* texture);
	void DrawVoxelTextured3D(Vector3 position, Texture* texture);
	void DrawVoxelTexturedFromSpriteSheet3D(IntVector3 position, Texture* texture, const AABB2D& spriteSheetBounds);
	void DrawVoxelTexturedFromSpriteSheet3D(IntVector3 position, Texture* texture, const AABB2D& spriteSheetBoundsZUp, const AABB2D& spriteSheetBoundsZDown, const AABB2D& spriteSheetBoundsSides);
	void DrawText2D(const Vector2& startBottomLeft,	const std::string& asciiText, float cellHeight, const RGBA& tint = RGBA::WHITE, float cellAspectScale = 1.f, const BitmapFont* font = nullptr);

	void DrawVertexArray3D_PCT( const Vertex3_PCT* vertexArray, int numVertexes, unsigned int primitiveDrawMode);
	
	unsigned int CreateVBO();
	void UpdateVBO(unsigned int vboID, Vertex3_PCT* vertexArray, int numVertexes);
	void DrawVBO3D_PCT(unsigned int vboID, int numVertexes, unsigned int primitiveDrawMode);
	void DestroyVBO(unsigned int vboID);

	void SetColor(const RGBA& clearColor);
	void ClearScreen(const RGBA& clearColor);
	void SetOrtho(const Vector2& bottomLeft, const Vector2& topRight);
	void SetPerspective(float fovYDegrees, float aspectRatio, float nearClipDistance, float farClipDistance);
	void Scale(float scaleFactor);
	void PushMatrix();
	void PopMatrix();
	void Translate(float xTranslation, float yTranslation, float zTranlation = 0.f);
	void Rotate(float rotationDegrees, float rotationOnXAxis, float rotationOnYAxis, float rotationOnZAxis);
	void Rotate(float rotationDegrees, const Vector3& rotationAxis);
	void LoadIdentity();
	void Clear();
	void SetAdditiveBlending();
	void SetAlphaBlending();
	void BindTexture2D(Texture* textureToBind);
	void SetDepthTesting(bool isDepthTesting);
	void SetLineWidth(float lineWidth);

	Texture* CreateOrGetTexture(const std::string& imageFilePath);
	BitmapFont* CreateOrGetFont(const std::string& bitmapFontName);

private:
	std::vector< Texture* >		m_loadedTextures;
	std::map< std::string, BitmapFont* > m_loadedFonts;

	Texture* CreateTexture(const std::string& imageFilePath);
	unsigned int CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
	Texture* GetTexture(const std::string& imageFilePath);

	BitmapFont* GetBitmapFont(const std::string& bitmapFontName);
	BitmapFont* CreateBitmapFont(const std::string& bitmapFontName);

	Texture* defaultWhiteTexture;
};