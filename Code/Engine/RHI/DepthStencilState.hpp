#pragma once
#include "Engine/RHI/DX11.hpp"

#if !defined(__RENDER_DEPTHSTENCILSTATE__)
#define __RENDER_DEPTHSTENCILSTATE__

class RHIDevice;

struct t_DepthStencilDesc
{
	bool depthWritingEnabled;
	bool depthTestEnabled;
};

class DepthStencilState
{
public:
	RHIDevice *device;
	ID3D11DepthStencilState *dxState;

	DepthStencilState(RHIDevice *owner, t_DepthStencilDesc const &desc);
	~DepthStencilState();

	inline bool IsValid() const {return (nullptr != dxState); }
};

#endif