#pragma once
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/Image.hpp"

class RHIOutput;

class Texture2D
{
public:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_dxBindFlags;
	RHIDevice *m_rhiDevice;
	ID3D11Texture2D *m_dxResource;
	ID3D11RenderTargetView *m_dxRTV;
	ID3D11ShaderResourceView *m_dxSRV;
	ID3D11DepthStencilView *m_dxDSV;
	ID3D11UnorderedAccessView *m_dxUAV;

	Texture2D( RHIDevice *device );
	Texture2D( RHIDevice *device, RHIOutput *output );
	Texture2D(RHIDevice *device, char const *filename);
	Texture2D(RHIDevice *device, Image const &image);
	Texture2D(RHIDevice *device, unsigned int w, unsigned int h, ImageFormat_e format);
	~Texture2D();

	bool LoadFromImage(Image const &img);
	void CreateViews();
	void CreateViews(DXGI_FORMAT dxFormat);

	unsigned int GetWidth();
	unsigned int GetHeight();

	inline bool IsValid() const { return (m_dxResource != nullptr); }
	inline bool IsRenderTarget() const { return (m_dxRTV != nullptr); }

	void Destroy();
};