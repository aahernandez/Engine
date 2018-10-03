#pragma once
#include "Engine/RHI/RHITypes.hpp"

#if defined(_DEBUG)
	#define INITGUID
#endif
#include <d3d11.h>
#include <DXGI.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }