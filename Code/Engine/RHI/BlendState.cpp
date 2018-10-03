#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/RHIDevice.hpp"

static D3D11_BLEND ToDXFactor(e_BlendFactor const factor)
{
	switch (factor)
	{
	case BLEND_ONE:
		return D3D11_BLEND_ONE;

	case BLEND_ZERO:
		return D3D11_BLEND_ZERO;

	case BLEND_SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;

	case BLEND_INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;

	default:
		//ASSERT OR DIE
		return D3D11_BLEND_ONE;
	}
}

BlendState::BlendState(RHIDevice *owner, bool enabled, e_BlendFactor const src_factor, e_BlendFactor dest_factor)
	: m_device(owner)
	, m_dxState(nullptr)
{
	D3D11_BLEND_DESC desc;
	MemZero(&desc);

	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	
	desc.RenderTarget[0].BlendEnable = enabled;

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = ToDXFactor(src_factor);
	desc.RenderTarget[0].DestBlend = ToDXFactor(dest_factor);

	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device *dxDevice = m_device->m_dxDevice;
	if (m_dxState != nullptr)
		m_dxState->Release();
	HRESULT hr = dxDevice->CreateBlendState(&desc, &m_dxState);

	if (FAILED(hr)) {
		//ASSERT OR DIE
		if (m_dxState != nullptr)
			m_dxState->Release();
		m_dxState = nullptr;
	}
}

BlendState::~BlendState()
{
	Destroy();
}

void BlendState::Destroy()
{
	if (m_dxState != nullptr)
		m_dxState->Release();
	m_dxState = nullptr;
}
