#pragma once
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/Core/RGBA.hpp"

class RasterState;
class BlendState;
class DepthStencilState;
class Texture2D;
class Sampler;
class ConstantBuffer;
class VertexBuffer;
class StructuredBuffer;
class ComputeJob;

class RHIDeviceContext
{
public:
	RHIDevice *m_device;
	ID3D11DeviceContext *m_dxContext;

	RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx);
	~RHIDeviceContext();

// 	void Draw(e_PrimitiveType topology, VertexBuffer *vbo);
	void Draw(e_PrimitiveType topology, VertexBuffer *vbo, IndexBuffer *ibo );

	void ClearState();
	void Flush();
	void ClearColorTarget(Texture2D *output, RGBA const &color);
	void ClearDepthTarget(Texture2D *output, float depth, unsigned char stencil);
	void SetColorTarget(Texture2D *target, Texture2D *depthStencilTarget = nullptr);
	void SetRasterState(RasterState *r);
	void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void SetShader(ShaderProgram *shader);
	void SetComputeShader(ComputeShaderProgram *shader);
	void SetTexture(unsigned int textureIndex, Texture2D *texture);
	void BindUAV(unsigned int textureIndex, Texture2D *texture);
	void UnbindUAV();
	void SetSampler(unsigned int samplerIndex, Sampler *sampler);
	void SetConstantBuffer(unsigned int index, ConstantBuffer *constantBuffer);
	void SetComputeConstantBuffer(unsigned int index, ConstantBuffer *constantBuffer);
	void SetStructuredBuffer(unsigned int bindPoint, StructuredBuffer *buffer);
	void DispatchComputeJob(ComputeJob *job);

	void SetDepthStencilState(DepthStencilState *dss);
	void Destroy();

	void SetBlendState(BlendState *bs);
};