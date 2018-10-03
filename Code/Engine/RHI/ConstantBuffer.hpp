#pragma once
#if !defined( __RHI_CONSTANT_BUFFER__ )
#define __RHI_CONSTANT_BUFFER__

#include "Engine/RHI/DX11.hpp"

class RHIDevice;
class RHIDeviceContext;

class ConstantBuffer
{
public:
	ID3D11Buffer *m_dxBuffer;
	size_t m_bufferSize;

	ConstantBuffer(RHIDevice *owner, void const *buffer, size_t const size);
	~ConstantBuffer();

	bool Update(RHIDeviceContext *context, void const *buffer);

	inline bool IsValid() const { return (m_dxBuffer != nullptr); }
	void Destroy();
};

#endif 
