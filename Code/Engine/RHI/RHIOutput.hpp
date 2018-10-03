#pragma once
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/Renderer/Window.hpp"

class RHIOutput
{
public:
	Window *m_window;
	RHIDevice *m_device;
	Texture2D *m_renderTarget;
	IDXGISwapChain *m_dxSwapChain;

	RHIOutput( RHIDevice *owner, IDXGISwapChain *swapChain, Window *wnd );
	~RHIOutput();

	void Present();
	void Close();

	Texture2D* GetRenderTarget();
	float GetWidth() const;
	float GetHeight() const;
	float GetAspectRatio() const;
	bool SetDisplayMode(e_RHIOutputMode const mode, float const width = 0.0f, float const height = 0.0f);
	bool SetWindowTitle(const char *title);


private:
	float m_windowWidth;
	float m_windowHeight;
	e_RHIOutputMode m_outputMode;

	bool CreateRenderTarget();
	void Destroy();
};