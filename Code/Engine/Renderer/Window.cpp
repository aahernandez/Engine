#include "Engine/Renderer/Window.hpp"

const int OFFSET_FROM_WINDOWS_DESKTOP = 50;

const char* APP_NAME = "Project";

LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	unsigned char keyCode = (unsigned char)wParam;
	switch (wmMessageCode)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
	{
		g_theApp->OnExitRequested();
		return 0;
	}

	case WM_KEYDOWN:
	{
		if (g_theInput)
			g_theInput->OnKeyDown(keyCode);
		break;
	}

	case WM_KEYUP:
	{
		if (g_theInput)
			g_theInput->OnKeyUp(keyCode);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		if (g_theInput)
			g_theInput->OnKeyDown(MOUSE_LEFT);
		break;
	}

	case WM_LBUTTONUP:
	{
		if (g_theInput)
			g_theInput->OnKeyUp(MOUSE_LEFT);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		if (g_theInput)
			g_theInput->OnKeyDown(MOUSE_RIGHT);
		break;
	}

	case WM_RBUTTONUP:
	{
		if (g_theInput)
			g_theInput->OnKeyUp(MOUSE_RIGHT);
		break;
	}
	
	case WM_MOUSEWHEEL:
	{
		if (g_theInput)
			g_theInput->SetWheelScrollDelta(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}

	}
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

Window::Window()
	:m_physicalHeight(800), m_physicalWidth(450), m_isOpen(false), m_hwnd(nullptr), m_DisplayDeviceContext(nullptr), m_mode(RENDEROUTPUT_WINDOWED)
{
	memset(&m_windowClassDescription, 0, sizeof(m_windowClassDescription));
}

Window::~Window() 
{
	Close();
}

void Window::SetClassDescription(UINT style, HICON icon, HCURSOR cursor, LPCTSTR className)
{
	m_windowClassDescription.style = style;
	m_windowClassDescription.hIcon = icon;
	m_windowClassDescription.hCursor = cursor;
	m_windowClassDescription.lpszClassName = className;
	m_windowClassDescription.cbSize = sizeof(m_windowClassDescription);
	m_windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure);
	m_windowClassDescription.hInstance = GetModuleHandle(NULL);
}

void Window::RegisterWindow()
{
	RegisterClassEx(&m_windowClassDescription);
}

void Window::Open()
{
	DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	int offsetWidth = (desktopRect.right - m_physicalWidth) / 2;
	int offsetHeight = (desktopRect.bottom - m_physicalHeight) / 2;
	int furtherOffsetWidth = 0;
	int furtherOffsetHeight = 0;

	RECT windowRect;

	if (m_mode == RENDEROUTPUT_WINDOWED)
	{
		windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU;
		windowRect = { offsetWidth + furtherOffsetWidth, offsetHeight + furtherOffsetHeight, offsetWidth + (long)m_physicalWidth - furtherOffsetWidth, offsetHeight + (long)m_physicalHeight - furtherOffsetHeight };
	}
	else if (m_mode == RENDEROUTPUT_BORDERLESS) 
	{
		windowStyleFlags = WS_POPUP;
		windowRect = { offsetWidth + furtherOffsetWidth, offsetHeight, offsetWidth + (long)m_physicalWidth - furtherOffsetWidth, offsetHeight + (long)m_physicalHeight - furtherOffsetHeight };
	}
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		m_windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		m_windowClassDescription.hInstance,
		NULL);

	if (m_mode == RENDEROUTPUT_WINDOWED)
	{
		ShowWindow(m_hwnd, SW_SHOW);
	}
	else if (m_mode == RENDEROUTPUT_BORDERLESS)
	{
		ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
	}
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	m_DisplayDeviceContext = GetDC(m_hwnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	m_isOpen = true;
}

void Window::Close()
{
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;
}

bool Window::IsOpen()
{
	return (IsWindow(m_hwnd) != 0);
}

UINT Window::GetWidth()
{
	return m_physicalWidth;
}

UINT Window::GetHeight()
{
	return m_physicalHeight;
}

void Window::SetClientSize(UINT width, UINT height)
{
	m_physicalWidth = width;
	m_physicalHeight = height;
}

void Window::SetWindowMode(e_RHIOutputMode newMode)
{
	m_mode = newMode;
}
