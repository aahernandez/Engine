#pragma once
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"

class RHIInstance
{
public:
	IDXGIDebug *m_dxDebug;
	HMODULE m_debugModule;

	~RHIInstance();

	static RHIInstance* GetInstance();
	static void Destroy();
	static void ReportObjects();
	bool CreateOutput(RHIDevice **outDevice, RHIDeviceContext **outContext, RHIOutput **outOutput, UINT const pxWidth, UINT const pxHeight, e_RHIOutputMode const initialMode);

private:
	RHIDevice *m_rhiDevice;

	RHIInstance();
};