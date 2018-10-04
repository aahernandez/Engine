#pragma once
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Grid.hpp"
#include <vector>

#define MATRIX_BUFFER_INDEX (0)
#define TIME_BUFFER_INDEX (1)
#define LIGHT_BUFFER_INDEX (2)

const float TAB_SPACES = 6.f;

struct t_MatrixBuffer
{
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;

	Vector4 eyePosition;
};

struct t_TimeBuffer
{
	float gameTime;
	float systemTime;
	float gameFrameTime;
	float systemFrameTime;
};

struct t_PointLightBuffer
{
	t_PointLightBuffer()
		: lightColor(1.f, 1.f, 1.f, 0.f)
		, lightPosition(0.f, 0.f, 0.f, 1.f)
		, attenuation(1.f, 0.f, 0.f, 0.f)
		, specAttenuation(1.f, 0.f, 0.f, 0.f)
	{}

	Vector4 lightColor;
	Vector4 lightPosition;
	Vector4 attenuation;
	Vector4 specAttenuation;
};

struct t_LightBuffer
{
	t_LightBuffer()
		: ambient(1.f, 1.f, 1.f, 1.f)	
		, specFactor(0.f)
		, specPower(1.f)
	{}

	t_PointLightBuffer pointLights[8];
	Vector4 ambient;
	float specFactor;
	float specPower;
	Vector2 unused;
};

struct t_BlendState
{
	bool enabled;
	e_BlendFactor srcFactor;
	e_BlendFactor destFactor;
};

class ShaderProgram;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class RasterState;
class ConstantBuffer;
class Font;
class MeshBuilder;
class UIButton;

class SimpleRenderer
{
public:
	Sampler *m_linearSampler;
	RHIDevice *m_rhiDevice;
	RHIOutput *m_rhiOutput;
	Texture2D *m_currentSetTexture;
	Texture2D *m_currentTarget;
	Texture2D *m_defaultDepthStencil;
	Texture2D *m_currentDepthStencil;
	Texture2D *m_whiteTexture;
	Texture2D *m_flatNormalTexture;
	RHIDeviceContext *m_rhiContext;
	RasterState *m_defaultRasterState;
	Font *m_font;
	AABB2D m_orthoProjection;

	t_BlendState m_blendState;
	BlendState *m_currentBlendState;
	t_DepthStencilDesc m_depthStencilDesc;
	DepthStencilState *m_depthStencilState;
	t_MatrixBuffer m_matrixData;
	ConstantBuffer *m_matrixCB;
	t_TimeBuffer m_timeData;
	ConstantBuffer *m_timeCB;
	t_LightBuffer m_lightData;
	ConstantBuffer *m_lightCB;

	ShaderProgram *m_colorShader;
	ShaderProgram *m_unlitShader;
	ShaderProgram *m_lightShader;
	ShaderProgram *m_skinWeightsShader;
	ShaderProgram *m_morphTargetShader;

	MeshBuilder *m_meshBuilder;

	Vector2 m_windowToScreenRatio;

	SimpleRenderer();
	~SimpleRenderer();

	void Setup(unsigned int width, unsigned int height, e_RHIOutputMode outputMode, char const *fontName);
	void Destroy();

	void LoadObjectFile(char const *modelFileName, std::vector<Vertex> &testObjectVertices, std::vector<DWORD> &testObjectIndices, std::string &fileName, float scale = 1.f);
	void SetRenderTarget(Texture2D *colorTarget, Texture2D *depthTarget = nullptr);
	void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void ClearColor(RGBA const &color);
	void ClearDepth(float depth = 1.f, unsigned char stencil = 0);
	void ClearTargetColor(Texture2D *target, RGBA const &color);
	void SetCameraMatrix(Matrix4 const &camMatrix);
	void SetModelMatrix(Matrix4 &model);
	void SetProjectionMatrix(Matrix4 &proj);
	void SetViewMatrix(Matrix4 &view);
	void SetViewMatrixTranslation(Vector3 translation);
	void SetViewMatrixRotation(float rollAboutX, float pitchAboutY,	float yawAboutZ);
	void SetViewMatrixTranslationAndRotation(Vector3 translation, float rollAboutX, float pitchAboutY, float yawAboutZ);
	void SetOrthoProjection(Vector2 const &bottomLeft, Vector2 const &topRight);
	void SetPerspectiveProjection(float const fovRadians, float aspectRatio, float const nz, float const fz);
	void EnableBlend(e_BlendFactor src, e_BlendFactor dest);
	void DisableBlend();
	void EnableDepthTest(bool isEnabled, bool isWrite);
	void EnableDepthWrite(bool isEnabled);
	void Present();
	void SetWindowTitle(const char *title);
	void DispatchComputeJob(ComputeJob *job);

	void SetShader(ShaderProgram *shader);
	void SetComputeShader(ComputeShaderProgram *shader);

	void SetTexture(unsigned int textureIndex, Texture2D *texture);
	void BindUAV(unsigned int textureIndex, Texture2D *texture);
	void UnbindUAV();
	inline void SetTexture(Texture2D *texture) { SetTexture(0, texture); }

	void SetSampler(unsigned int samplerIndex, Sampler *sampler);
	inline void SetSampler(Sampler *sampler) { SetSampler(0, sampler); }

	// other aliases
	inline void SetDiffuse(Texture2D *texture) { SetTexture(0, (texture == nullptr) ? m_whiteTexture : texture); }
	inline void SetNormal(Texture2D *texture) { SetTexture(1, (texture == nullptr) ? m_flatNormalTexture : texture); }
	inline void SetSpec(Texture2D *texture) { SetTexture(2, (texture == nullptr) ? m_whiteTexture : texture); }

	void SetAmbientLight(float intensity, RGBA const &color = RGBA::WHITE);
	void SetSpecularConstants(float const specPower, float const specFactor = 1.0f);

	void EnablePointLight(int index, Vector3 const &pos, RGBA const &color, float intensity = 1.0f, Vector3 const &attenuation = Vector3(0, 0, 1), Vector3 const &specAttenuation = Vector3(0, 0, 1));
	inline void DisablePointLight(int index) { EnablePointLight(index, Vector3(0.f, 0.f, 0.f), RGBA::WHITE, 0.0f, Vector3(1.f, 0.f, 0.f) ); }

	void SetConstantBuffer(unsigned int idx, ConstantBuffer *cb);
	void SetComputeConstantBuffer(unsigned int index, ConstantBuffer *cb);
	void SetStructuredBuffer(unsigned int index, StructuredBuffer *sb);

	void Draw(e_PrimitiveType topology, VertexBuffer *vbo, IndexBuffer *ibo);
	void DrawVertexBufferArray(e_PrimitiveType topology, std::vector<VertexBuffer *> m_vertexBufferArray, std::vector<IndexBuffer *> m_indexBufferArray);
	void DrawMeshBuilder();
	void CreateTexturedQuad2D(Vector2 bottomLeft, Vector2 topRight, Vector2 uvBottomLeft, Vector2 uvTopRight, RGBA color, VertexBuffer** vertexBuffer, IndexBuffer** indexBuffer);
	VertexBuffer* CreateQuad2DVertexBuffer(float x, float y, float z, RGBA color);
	VertexBuffer* CreateTexturedQuad2DVertexBuffer(float x, float y, float z, float vec2XMin, float vec2YMin, float vec2XMax, float vec2YMax, RGBA color);
	VertexBuffer* CreateTexturedQuad3DVertexBuffer(Vector3 bottomLeft, Vector3 topRight, Vector3 uvBottomLeft, Vector3 uvTopRight, RGBA color);
	VertexBuffer* CreateLine3DVertexBuffer(Vector3 const &startPos, Vector3 const &endPos, RGBA const &color = RGBA::WHITE);
	VertexBuffer* CreateQuad3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color = RGBA::WHITE);

	//2D
	void DrawText2D(char const *string, Vector2 position, RGBA color = RGBA::WHITE, float scale = 1.f, Font *font = nullptr);
	void DrawText2DCenteredOnPosition(char const *string, Vector2 position, RGBA color = RGBA::WHITE, float scale = 1.f, Font *font = nullptr);
	void DrawTextList2DCenteredOnPosition(std::vector<std::string>& stringList, Vector2 position, Vector2 positionDelta, RGBA color = RGBA::WHITE, float scale = 1.f, Font *font = nullptr);
	void DrawText2DCenteredAndScaledToFitWithinBounds(char const *string, AABB2D bounds, RGBA color = RGBA::WHITE, Font *font = nullptr);
	void GetTextQuad2DCenteredAndScaledToFitWithinBounds(AABB2D bounds, RGBA color, Font *font, char glyph, VertexBuffer **vertexBuffer, IndexBuffer **indexBuffer);
	void DrawText2DScaledWithAspectRatio(Vector2 postion, float scaleX, float scaleY, RGBA color, Font *font, char const *string);
	void DrawQuad2D(Vector2 bottomLeft, Vector2 topRight, RGBA color);
	void DrawQuad2D(Vector2 bottomLeft, Vector2 topRight, Vector2 uvBottomLeft, Vector2 uvTopRight, RGBA color);
	void DrawQuad2DTextured(Vector2 bottomLeft, Vector2 topRight, Texture2D *texture, RGBA color = RGBA::WHITE);
	void DrawQuad2DTextured(const AABB2D& bounds, const AABB2D uvBounds, Texture2D *texture, float orientation = 0.f, const RGBA& color = RGBA::WHITE);
	void DrawQuad2DTextured(const AABB2D& bounds, Texture2D* texture, RGBA color = RGBA::WHITE);
	void CreateQuad2DTextured(const AABB2D& bounds, const AABB2D uvBounds, Texture2D *texture, const RGBA& color, VertexBuffer **vertexBuffer, IndexBuffer **indexBuffer);
	void DrawLineRotated2D(const Vector2& startPoint, const Vector2& endPoint, float lineThickness, const RGBA& startColor = RGBA::WHITE, const RGBA& endColor = RGBA::WHITE, const float& rotation = 0.f, const Vector2& newMidPoint = Vector2::ZERO);
	void DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, float lineThickness, const RGBA& startColor = RGBA::WHITE, const RGBA& endColor = RGBA::WHITE);
	void DrawGrid2D(const Grid& grid, const AABB2D& gridBounds, float lineThickness = 1.f, const RGBA& color = RGBA::WHITE);
	void DrawDisc2D(const Vector2& center, const float radius, const RGBA& color, int numVertices);
	void DrawDebugQuad2D(const AABB2D& bounds, float edgeThickness, const RGBA& interiorColor, const RGBA& edgeColor);
	void DrawDebugCircle2D(const Vector2& position, float radius, float edgeThickness, int numSides, const RGBA& interiorColor, const RGBA& edgeColor);
	void DrawCircleHollow2D(const Vector2& position, float radius, float edgeThickness, int numSides, const RGBA& edgeColor);
	void AddQuadToMeshBuilder(Vector2 bottomLeft, Vector2 topRight, RGBA color = RGBA::WHITE, Vector2 uvBottomLeft = Vector2::ZERO, Vector2 uvTopRight = Vector2::ONE, int startingIndex = 0);
	void AddQuadToMeshBuilder(AABB2D quadBounds, RGBA color = RGBA::WHITE, Vector2 uvBottomLeft = Vector2::ZERO, Vector2 uvTopRight = Vector2::ONE, int startingIndex = 0);
	
	void DrawUIButton(const UIButton& uiButton);
	
	//3D
	void DrawOriginLines3D();
	void DrawText3D(Vector3 position, float scale, RGBA color, Font *font, char const *string);
	void DrawQuad3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color = RGBA::WHITE);
	void DrawQuadDoubleSided3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color = RGBA::WHITE);
	void DrawCube3D(Vector3 const &position, RGBA const &color = RGBA::WHITE, float scale = 1.f);
	void DrawLine3D(Vector3 const &startPos, Vector3 const &endPos, RGBA const &color = RGBA::WHITE);
	void DrawCylinder3D(Vector3 const &position, float radius, float height, int numVertexes, const RGBA& color = RGBA::WHITE);
	void DrawDisc3D(Vector3 const &position, float radius, int numVertexes, const RGBA& color = RGBA::WHITE);
};