#pragma once
#include "Engine/RHI/DX11.hpp"

class RHIDevice;
class RHIDeviceContext;

class StructuredBuffer
{
public:
	unsigned int obj_count;
	unsigned int obj_size;
	size_t buffer_size;
	ID3D11Buffer *dxBuffer;
	ID3D11ShaderResourceView *dxSRV;

	StructuredBuffer(RHIDevice *owner, void const *initialData, unsigned int objectSize, unsigned int objectCount);
	~StructuredBuffer();

	bool CreateViews(RHIDevice *owner);

	bool Update(RHIDeviceContext *context, void const *buffer);

	inline bool IsValid() const { return (dxBuffer != nullptr); }
};