#pragma once
#include "Engine/RHI/DX11.hpp"
#include <vector>
class RHIDevice;

class IndexBuffer
{
public:
	ID3D11Buffer *m_dxBuffer;
	unsigned int const m_indexCount;

	IndexBuffer(RHIDevice *owner, DWORD const *indices, unsigned int const indexCount);
	IndexBuffer(RHIDevice *owner, std::vector<DWORD> const &indices);
	IndexBuffer(RHIDevice *owner, std::vector<DWORD> const &indices, int startIndex, int count);
// 	IndexBuffer(RHIDevice *owner, std::vector<unsigned int> const &indices);
	~IndexBuffer();

	inline bool IsValid() const { return (m_dxBuffer != nullptr); }

	void Destroy();
};

