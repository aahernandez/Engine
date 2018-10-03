#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/RHIOutput.hpp"

Texture2D::Texture2D(RHIDevice *device)
	: m_rhiDevice(device)
	, m_dxResource(nullptr)
	, m_dxRTV(nullptr)
	, m_dxSRV(nullptr)
	, m_dxDSV(nullptr)
	, m_dxUAV(nullptr)
	, m_width(0)
	, m_height(0)
	, m_dxBindFlags(0U)
{}

Texture2D::Texture2D(RHIDevice *device, char const *filename)
	: Texture2D(device)
{
	Image image;
	if (!image.LoadFromFile(filename)) {
		return;
	}
	LoadFromImage(image);
}

Texture2D::Texture2D(RHIDevice *device, Image const &image)
	: Texture2D(device)
{
	LoadFromImage(image);
}

Texture2D::Texture2D(RHIDevice *device, RHIOutput *output)
	: Texture2D(device)
{
	if (output != nullptr) {
		ID3D11Texture2D *backBuffer = nullptr;
		output->m_dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (backBuffer != nullptr) {
			if (m_dxResource != nullptr)
				m_dxResource->Release();
			m_dxResource = backBuffer;
			backBuffer = nullptr;

			D3D11_TEXTURE2D_DESC desc;
			m_dxResource->GetDesc(&desc);

			m_width = desc.Width;
			m_height = desc.Height;
			m_dxBindFlags = desc.BindFlags;

			CreateViews();
		}
	}
}

Texture2D::Texture2D(RHIDevice *device, unsigned int w, unsigned int h, ImageFormat_e format)
	: Texture2D(device)
{
	// default usage - this is going to be written to by the GPU
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	m_width = w;
	m_height = h;

	DXGI_FORMAT dx_format;
	unsigned int dx_bindings = 0U;
	switch (format) {
	case IMAGEFORMAT_RGBA8:
		dx_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dx_bindings = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		break;
	case IMAGEFORMAT_RGBA8_UAV:
		dx_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dx_bindings = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		break;
	case IMAGEFORMAT_D24S8:
		dx_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dx_bindings = D3D11_BIND_DEPTH_STENCIL;
		break;
	default:
		return;
	};

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = m_width;
	tex_desc.Height = m_height;
	tex_desc.MipLevels = 1;               
	tex_desc.ArraySize = 1;                  
	tex_desc.Usage = usage;
	tex_desc.Format = dx_format;
	tex_desc.BindFlags = dx_bindings;
	tex_desc.CPUAccessFlags = 0U;
	tex_desc.MiscFlags = 0U;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	ID3D11Device *dx_device = device->m_dxDevice;
	HRESULT hr = dx_device->CreateTexture2D(&tex_desc, nullptr, &m_dxResource);

	if (SUCCEEDED(hr)) 
	{
		m_dxBindFlags = dx_bindings;
		CreateViews(dx_format);
	}
}

Texture2D::~Texture2D()
{
	Destroy();
}

unsigned int Texture2D::GetWidth()
{
	return m_width;
}

unsigned int Texture2D::GetHeight()
{
	return m_height;
}

void Texture2D::Destroy()
{
	if (IsValid())
	{
		if (m_dxUAV != nullptr)
			m_dxUAV->Release();
		m_dxUAV = nullptr;

		if (m_dxRTV != nullptr)
			m_dxRTV->Release();
		m_dxRTV = nullptr;

		if (m_dxResource != nullptr)
			m_dxResource->Release();
		m_dxResource = nullptr;

		if (m_dxSRV != nullptr)
			m_dxSRV->Release();
		m_dxSRV = nullptr;

		if (m_dxDSV != nullptr)
			m_dxDSV->Release();
		m_dxDSV = nullptr;
	}
}

bool Texture2D::LoadFromImage(Image const &img)
{
	m_width = img.GetWidth();
	m_height = img.GetHeight();
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Width = img.GetWidth();     
	desc.Height = img.GetHeight();   
	desc.MipLevels = 1;   
	desc.ArraySize = 1;   
	desc.Usage = D3D11_USAGE_IMMUTABLE;            
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;   
	desc.CPUAccessFlags = 0U;                      
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	m_dxBindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA data;
	memset( &data, 0, sizeof(data));

	data.pSysMem = img.m_buffer;
	data.SysMemPitch = img.GetWidth() * 4;

	ID3D11Device *dxDevice = m_rhiDevice->m_dxDevice;
	if (m_dxResource != nullptr)
		m_dxResource->Release();
	HRESULT hr = dxDevice->CreateTexture2D( &desc, &data, &m_dxResource);

	if (SUCCEEDED(hr)) 
	{
		CreateViews();
		return true;
	}
	else
	{
		return false;
	}
}

void Texture2D::CreateViews()
{
	ID3D11Device *dd = m_rhiDevice->m_dxDevice;

	if (m_dxBindFlags & D3D11_BIND_RENDER_TARGET)
	{
		dd->CreateRenderTargetView(m_dxResource, nullptr, &m_dxRTV);
	}

	if (m_dxBindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		dd->CreateShaderResourceView(m_dxResource, nullptr, &m_dxSRV);
	}

	if (m_dxBindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		dd->CreateDepthStencilView(m_dxResource, nullptr, &m_dxDSV);
	}
}

void Texture2D::CreateViews(DXGI_FORMAT dxFormat)
{
	ID3D11Device *dd = m_rhiDevice->m_dxDevice;

	if (m_dxBindFlags & D3D11_BIND_RENDER_TARGET) 
	{
		dd->CreateRenderTargetView(m_dxResource, nullptr, &m_dxRTV);
	}

	if (m_dxBindFlags & D3D11_BIND_SHADER_RESOURCE) 
	{
		dd->CreateShaderResourceView(m_dxResource, nullptr, &m_dxSRV);
	}

	if(m_dxBindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		MemZero( &uavDesc );

		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Format = dxFormat;

		dd->CreateUnorderedAccessView(m_dxResource, &uavDesc, &m_dxUAV);
	}

	if (m_dxBindFlags & D3D11_BIND_DEPTH_STENCIL) 
	{
		dd->CreateDepthStencilView(m_dxResource, nullptr, &m_dxDSV);
	}
}
