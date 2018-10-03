#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/RHIDevice.hpp"

Sampler::Sampler(RHIDevice *owner)
	: m_device(owner)
	, m_dxSampler(nullptr)
{}

Sampler::Sampler(RHIDevice *owner, e_FilterMode min, e_FilterMode mag)
	: Sampler(owner)
{
	CreateSampler(min, mag);
}

Sampler::~Sampler()
{
	Destroy();
}

static D3D11_FILTER DXGetFilterMode(e_FilterMode min, e_FilterMode mag)
{
	if (min == FILTER_POINT) {
		if (mag == FILTER_POINT) {
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else {
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
	}
	else {
		if (mag == FILTER_POINT) {
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else {
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
	}

}

bool Sampler::CreateSampler(e_FilterMode min, e_FilterMode mag)
{
	D3D11_SAMPLER_DESC desc;
	memset( &desc, 0, sizeof(D3D11_SAMPLER_DESC) );

	desc.Filter = DXGetFilterMode(min, mag);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MinLOD = 0;
	desc.MaxLOD = 0;

	ID3D11Device *dx_device = m_device->m_dxDevice;
	HRESULT hr = dx_device->CreateSamplerState(&desc, &m_dxSampler);

	return SUCCEEDED(hr);
}

void Sampler::Destroy()
{
	if (m_dxSampler != nullptr)
		m_dxSampler->Release();
	m_dxSampler = nullptr;
}
