#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

static RHIInstance *g_renderInstance = nullptr;

RHIInstance::RHIInstance()
{
	m_dxDebug = nullptr;
	m_debugModule = nullptr;

#if defined(_DEBUG)
	m_debugModule = LoadLibraryExA( "dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32 );
	typedef HRESULT(WINAPI *GetDebugModuleCB)(REFIID, void**);
	GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(m_debugModule, "DXGIGetDebugInterface");
	HRESULT hr = cb(__uuidof(IDXGIDebug), (void**)&m_dxDebug);
	ASSERT_OR_DIE(SUCCEEDED(hr), "Failed to obtain IDXGIDebug Info for Reporting Live Objects.");
#endif
}

RHIInstance::~RHIInstance()
{
}

bool RHIInstance::CreateOutput(RHIDevice **outDevice, RHIDeviceContext **outContext, RHIOutput **outOutput, UINT const pxWidth, UINT const pxHeight, e_RHIOutputMode const initialMode)
{
	Window *window = new Window();

	window->SetClientSize((int)(pxWidth), (int)(pxHeight));
	window->SetWindowMode(initialMode);
	window->SetClassDescription(CS_OWNDC, NULL, NULL, TEXT("Simple Window Class"));
	window->RegisterWindow();
	window->Open();

	UINT deviceFlags = 0U;
#if defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapDesc;
	memset(&swapDesc, 0, sizeof(swapDesc));

	swapDesc.BufferCount = 2;                                    

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       
		| DXGI_USAGE_BACK_BUFFER;
	swapDesc.OutputWindow = window->m_hwnd;                        
	swapDesc.SampleDesc.Count = 1;                               

																  
	swapDesc.Windowed = TRUE;                                    
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     
	swapDesc.BufferDesc.Width = window->GetWidth();
	swapDesc.BufferDesc.Height = window->GetHeight();

	ID3D11Device *dxDevice;
	ID3D11DeviceContext *dxContext;
	IDXGISwapChain *dxSwapChain;

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,  
		nullptr,                   
		deviceFlags,              
		nullptr,                   
		0U,                        
		D3D11_SDK_VERSION,         
		&swapDesc,                
		&dxSwapChain,             
		&dxDevice,                
		nullptr,                   
		&dxContext);    

	if (SUCCEEDED(hr)) {
		RHIDevice *device = new RHIDevice(this, dxDevice);
		RHIDeviceContext *context = new RHIDeviceContext(device, dxContext);
		RHIOutput *output = new RHIOutput(device, dxSwapChain, window);

		device->m_immediateContext = context;

		*outDevice = device;
		*outContext = context;
		*outOutput = output;

		dxDevice = nullptr;
		dxContext = nullptr;
		dxSwapChain = nullptr;
		device = nullptr;
		context = nullptr;
		output = nullptr;

		return true;
	}
	else {
		delete window;
		return false;
	}
}

RHIInstance* RHIInstance::GetInstance()
{
	if (g_renderInstance == nullptr) {
		g_renderInstance = new RHIInstance();
	}

	return g_renderInstance;
}

void RHIInstance::Destroy()
{
	ReportObjects();

	if (g_renderInstance->m_dxDebug != nullptr)
	{
		DX_SAFE_RELEASE(g_renderInstance->m_dxDebug);
		if (g_renderInstance->m_debugModule != nullptr)
		{
			FreeLibrary(g_renderInstance->m_debugModule);
			g_renderInstance->m_debugModule = nullptr;
		}
	}

	delete g_renderInstance;
	g_renderInstance = nullptr;
}

void RHIInstance::ReportObjects()
{
#if defined(_DEBUG)
	g_renderInstance->m_dxDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif
}
