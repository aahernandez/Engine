#pragma once
#if !defined( __RHI_VERTEX_BUFFER__ )
#define __RHI_VERTEX_BUFFER__

#include "Engine/RHI/DX11.hpp"
#include "Engine/Math/Vertex.hpp"
#include <vector>

class RHIDevice;

class VertexBuffer
{
public:
	ID3D11Buffer *m_dxBuffer;
	unsigned int const m_vertexCount;

	VertexBuffer(RHIDevice *owner, Vertex const *vertices, unsigned int const vertexCount);
	VertexBuffer(RHIDevice *owner, std::vector<Vertex> const &vertices);
	VertexBuffer(RHIDevice *owner, std::vector<Vertex> const &vertices, int startIndex, int count);
	~VertexBuffer();

	inline bool IsValid() const { return (m_dxBuffer != nullptr); }

	void Destroy();
};

#endif 
