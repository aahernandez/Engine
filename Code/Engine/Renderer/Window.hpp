#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"
#include "Engine/RHI/RHITypes.hpp"

class Window
{
public:
	HWND m_hwnd;
	HDC m_DisplayDeviceContext;

	Window();
	~Window();

	void SetClientSize(UINT, UINT);
	void SetWindowMode(e_RHIOutputMode newMode);
	void RegisterWindow();
	void SetClassDescription(UINT, HICON, HCURSOR, LPCTSTR);
	void Open();
	void Close();
	bool IsOpen();
	UINT GetWidth();
	UINT GetHeight();

private:
	WNDCLASSEX m_windowClassDescription;
	UINT m_physicalHeight;
	UINT m_physicalWidth;
	bool m_isOpen;
	e_RHIOutputMode m_mode;
};
