#pragma once
#include "Engine/RHI/DX11.hpp"

class RHIDevice;

class  RasterState
{
public:
	RHIDevice *m_device;
	ID3D11RasterizerState *m_dxState;

	RasterState(RHIDevice *owner);
	~RasterState();

	inline bool IsValid() const { return (nullptr != m_dxState); }
	void Destroy();
};