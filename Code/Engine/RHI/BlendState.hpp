#pragma once
#include "Engine/RHI/DX11.hpp"

class RHIDevice;

enum e_BlendFactor
{
	BLEND_ONE,
	BLEND_ZERO,

	BLEND_SRC_ALPHA,
	BLEND_INV_SRC_ALPHA,

	BLEND_SRC_COLOR,
	BLEND_DEST_COLOR,
};

class BlendState
{
public:
	RHIDevice *m_device;
	ID3D11BlendState *m_dxState;

	BlendState(RHIDevice *owner, bool enabled, e_BlendFactor const srcFactor = BLEND_ONE, e_BlendFactor destFactor = BLEND_ZERO);
	~BlendState();

	void Destroy();

	inline bool IsValid() const {return (nullptr != m_dxState); }
};