#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"

IndexBuffer::IndexBuffer(RHIDevice *owner, DWORD const *indices, unsigned int const indexCount)
	: m_indexCount(indexCount)
{
	// First, describe the buffer
	D3D11_BUFFER_DESC ibDesc;
	memset(&ibDesc, 0, sizeof(ibDesc));

	ibDesc.ByteWidth = sizeof(DWORD) * indexCount;  // How much data are we putting into this buffer
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	ibDesc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer

														// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = indices;

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&ibDesc, &initial_data, &m_dxBuffer);
}

IndexBuffer::IndexBuffer(RHIDevice *owner, std::vector<DWORD> const &indices)
	: m_indexCount(indices.size())
{
	// First, describe the buffer
	D3D11_BUFFER_DESC ibDesc;
	memset(&ibDesc, 0, sizeof(ibDesc));

	ibDesc.ByteWidth = sizeof(DWORD) * m_indexCount;  // How much data are we putting into this buffer
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														 // for limitations/strenghts of each, see;  
														 //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	ibDesc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer

													   // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = &indices[0];

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&ibDesc, &initial_data, &m_dxBuffer);
}

// IndexBuffer::IndexBuffer(RHIDevice *owner, std::vector<unsigned int> const &indices)
// 	: m_indexCount(indices.size())
// {
// 	D3D11_BUFFER_DESC ibDesc;
// 	memset(&ibDesc, 0, sizeof(ibDesc));
// 
// 	ibDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;  // How much data are we putting into this buffer
// 	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
// 	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
// 														 // for limitations/strenghts of each, see;  
// 														 //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
// 	ibDesc.StructureByteStride = sizeof(Vertex);       // How large is each element in this buffer
// 
// 													   // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
// 	D3D11_SUBRESOURCE_DATA initial_data;
// 	memset(&initial_data, 0, sizeof(initial_data));
// 	initial_data.pSysMem = &indices[0];
// 
// 	m_dxBuffer = nullptr;
// 	owner->m_dxDevice->CreateBuffer(&ibDesc, &initial_data, &m_dxBuffer);
// }

IndexBuffer::IndexBuffer(RHIDevice *owner, std::vector<DWORD> const &indices, int startIndex, int count)
	: m_indexCount(count)
{
	D3D11_BUFFER_DESC ibDesc;
	memset(&ibDesc, 0, sizeof(ibDesc));

	ibDesc.ByteWidth = sizeof(DWORD) * m_indexCount; 
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;      
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;            								
	ibDesc.StructureByteStride = sizeof(DWORD);     
													 
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = &indices[startIndex];

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&ibDesc, &initial_data, &m_dxBuffer);
}

IndexBuffer::~IndexBuffer()
{
	Destroy();
}

void IndexBuffer::Destroy()
{
	if (m_dxBuffer != nullptr)
		m_dxBuffer->Release();
	m_dxBuffer = nullptr;
}
