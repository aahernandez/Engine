#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"

VertexBuffer::VertexBuffer(RHIDevice *owner, Vertex const *vertices, unsigned int const vertex_count)
	: m_vertexCount(vertex_count)
{
	// First, describe the buffer
	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = sizeof(Vertex) * vertex_count;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vb_desc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer

														  // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = vertices;

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&vb_desc, &initial_data, &m_dxBuffer);
}

VertexBuffer::VertexBuffer(RHIDevice *owner, std::vector<Vertex> const &vertices)
	: m_vertexCount(vertices.size())
{
	// First, describe the buffer
	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = sizeof(Vertex) * m_vertexCount;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vb_desc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer

														// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = &vertices[0];

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&vb_desc, &initial_data, &m_dxBuffer);
}

VertexBuffer::VertexBuffer(RHIDevice *owner, std::vector<Vertex> const &vertices, int startIndex, int count)
	: m_vertexCount(count)
{
	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = sizeof(Vertex) * m_vertexCount;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vb_desc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer

														// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = &vertices[startIndex];

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&vb_desc, &initial_data, &m_dxBuffer);
}

VertexBuffer::~VertexBuffer()
{
	Destroy();
}

void VertexBuffer::Destroy()
{
	if (m_dxBuffer != nullptr)
		m_dxBuffer->Release();
	m_dxBuffer = nullptr;
}
