#include "Engine/RHI/RHIOutput.hpp"

RHIOutput::RHIOutput(RHIDevice *owner, IDXGISwapChain *swapChain, Window *wnd)
	: m_window(wnd)
	, m_dxSwapChain(swapChain)
	, m_renderTarget(nullptr)
{
	m_device = owner;
	CreateRenderTarget();
}

RHIOutput::~RHIOutput()
{
	Destroy();
}

void RHIOutput::Present()
{
	m_dxSwapChain->Present(0, 0);
}

void RHIOutput::Close()
{
	m_window->Close();
}

Texture2D* RHIOutput::GetRenderTarget()
{
	return m_renderTarget;
}

float RHIOutput::GetWidth() const
{
	return (float)m_window->GetWidth();
}

float RHIOutput::GetHeight() const
{
	return (float)m_window->GetHeight();
}

float RHIOutput::GetAspectRatio() const
{
	return (m_windowWidth / m_windowHeight);
}

bool RHIOutput::SetDisplayMode(e_RHIOutputMode const mode, float const width, float const height)
{
	m_outputMode = mode;
	m_windowWidth = width;
	m_windowHeight = height;
	m_window->SetClientSize((int)(m_windowWidth), (int)(m_windowHeight));
	SetWindowPos(m_window->m_hwnd, 0, 0, 0, (int)m_windowWidth, (int)m_windowHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	return true;
}

bool RHIOutput::SetWindowTitle(const char* title)
{

	wchar_t newWindowTitle[30];
	mbstowcs(newWindowTitle, title, strlen(title) + 1);

	SetWindowText(m_window->m_hwnd, newWindowTitle);
	return true;
}

bool RHIOutput::CreateRenderTarget()
{
	if (m_renderTarget != nullptr)
		delete m_renderTarget;
	m_renderTarget = nullptr;

	m_renderTarget = new Texture2D(m_device, this);
	return m_renderTarget->IsValid();
}

void RHIOutput::Destroy()
{
	delete m_window;
	m_window = nullptr;

	if (m_dxSwapChain != nullptr)
		m_dxSwapChain->Release();
	m_dxSwapChain = nullptr;

	m_device = nullptr;

	m_renderTarget = nullptr;
}
