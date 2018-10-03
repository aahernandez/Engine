#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/RHIDevice.hpp"

RasterState::RasterState(RHIDevice *owner)
	: m_device(owner)
	, m_dxState(nullptr)
{
	D3D11_RASTERIZER_DESC desc;
	memset( &desc, 0, sizeof(D3D11_RASTERIZER_DESC) );

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = true;
	desc.AntialiasedLineEnable = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.f;
	desc.SlopeScaledDepthBias = 0.f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;

	m_device->m_dxDevice->CreateRasterizerState(&desc, &m_dxState);
}

RasterState::~RasterState()
{
	Destroy();
}

void RasterState::Destroy()
{
	DX_SAFE_RELEASE(m_dxState);
}
