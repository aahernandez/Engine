#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/RHI/RHIDevice.hpp"

DepthStencilState::DepthStencilState(RHIDevice *owner, t_DepthStencilDesc const &desc)
	: device(owner)
	, dxState(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC dxDesc;
	MemZero(&dxDesc);

	dxDesc.DepthEnable = desc.depthWritingEnabled || desc.depthTestEnabled;
	dxDesc.DepthWriteMask = desc.depthWritingEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dxDesc.DepthFunc = desc.depthTestEnabled ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;
	dxDesc.StencilEnable = FALSE;

	HRESULT result = device->m_dxDevice->CreateDepthStencilState(&dxDesc, &dxState);
	UNREFERENCED_PARAMETER(result);
}

DepthStencilState::~DepthStencilState()
{
	DX_SAFE_RELEASE(dxState);
}
