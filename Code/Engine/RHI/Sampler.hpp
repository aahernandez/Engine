#pragma once
#if !defined( __RENDER_SAMPLER__ )
#define __RENDER_SAMPLER__

#include "Engine/RHI/DX11.hpp"

enum e_FilterMode
{
	FILTER_POINT,
	FILTER_LINEAR,
};

class RHIDevice;

class Sampler
{
public:
	RHIDevice *m_device;
	ID3D11SamplerState *m_dxSampler;
	
	Sampler(RHIDevice *owner);
	Sampler(RHIDevice *owner, e_FilterMode minFilter, e_FilterMode magFilter);
	~Sampler();

	bool CreateSampler(e_FilterMode minFilter, e_FilterMode magFilter);

	inline bool IsValid() const { return (nullptr != m_dxSampler); }
	void Destroy();
};

#endif 
