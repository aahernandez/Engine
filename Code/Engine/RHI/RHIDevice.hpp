#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Vertex.hpp"
#include <string>
#include <vector>

class RHIDeviceContext;
class RHIOutput;
class ShaderProgram;
class Window;
class Texture2D;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class RHIInstance;
class ComputeShaderProgram;

class RHIDevice
{
public:
	RHIInstance *m_instanceOwner; 
	ID3D11Device *m_dxDevice;
	IDXGISwapChain *m_dxSwapChain;
	RHIDeviceContext *m_immediateContext;
	ID3D11DeviceContext *m_dxContext;

	RHIDevice( RHIInstance *owner, ID3D11Device *dxd );
	~RHIDevice();

	RHIDeviceContext* GetImmediateContext();
	void D3D11Cleanup();

	ShaderProgram* CreateShaderFromHLSLFile(char const* filename);
	ComputeShaderProgram* CreateComputeShaderFromHLSLFile(char const* filename);
	VertexBuffer* CreateVertexBuffer( Vertex *vertices, unsigned int vertexCount);
	VertexBuffer* CreateVertexBuffer(std::vector<Vertex> const &vertices);
	VertexBuffer* CreateVertexBuffer(std::vector<Vertex> const &vertices, int startIndex, int count);
	IndexBuffer* CreateIndexBuffer(DWORD *indices, unsigned int indexCount);
	IndexBuffer* CreateIndexBuffer(std::vector<DWORD> const &indices);
	IndexBuffer* CreateIndexBuffer(std::vector<DWORD> const &indices, int startIndex, int count);
// 	IndexBuffer* CreateIndexBuffer(std::vector<unsigned int> const &indices);
};
