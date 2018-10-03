#include "Engine/Input/InputSystem.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

InputSystem::InputSystem()
	: m_doesAppHaveFocus(true)
	, m_isMouseCursorHidden(true)
	, lastKeyCode(0)
	, m_wheelScrollDelta(0)
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::UpdateInputState()
{
	UpdateKeyboard();
	UpdateController();
	RunMessagePump();
}

bool InputSystem::IsKeyDown(int keyCode)
{
	return m_keyStates[keyCode].m_isDown;
}

bool InputSystem::WasKeyJustPressed(int keyCode)
{
	bool keyWasDown = m_keyStates[keyCode].m_wasDown;
	bool keyIsDown = m_keyStates[keyCode].m_isDown;
	return keyIsDown && !keyWasDown;
}

void InputSystem::OnKeyDown(int keyCode)
{
	m_keyStates[keyCode].m_isDown = true;
	lastKeyCode = keyCode;
}

void InputSystem::OnKeyUp(int keyCode)
{
	m_keyStates[keyCode].m_isDown = false;
}

int InputSystem::GetLastKeyCode()
{
	int returnCode = lastKeyCode;
	lastKeyCode = 0;
	return returnCode;
}

bool InputSystem::WasMouseJustScrolled()
{
	return m_wheelScrollDelta != 0;
}

int InputSystem::PopScrollDelta()
{
	int tempDelta = m_wheelScrollDelta;
	m_wheelScrollDelta = 0;
	return tempDelta;
}

XboxControl& InputSystem::GetController(int controllerID)
{
	return m_controllers[controllerID];
}

void InputSystem::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

void InputSystem::InitializeControllers()
{
	for (int controllerIndex = 0; controllerIndex < MAX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].controllerNumber = controllerIndex;
	}
}

void InputSystem::UpdateKeyboard()
{
	for (int keyCode = 0; keyCode < NUM_KEYS; ++keyCode)
	{
		KeyState& keyState = m_keyStates[keyCode];
		keyState.m_wasDown = keyState.m_isDown;
	}
}

void InputSystem::UpdateController()
{
	for (int controllerIndex = 0; controllerIndex < MAX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].Update();
	}
}

Vector2 InputSystem::GetMouseScreenCoords()
{
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);
	Vector2 cursorScreenPosFloats((float)cursorPoint.x, (float)cursorPoint.y);
	return cursorScreenPosFloats;
}


Vector2 InputSystem::GetMouseWindowCoords()
{
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);
	ScreenToClient(GetActiveWindow(), &cursorPoint);
	Vector2 cursorScreenPosFloats((float)cursorPoint.x, (float)cursorPoint.y);
	return cursorScreenPosFloats;
}

void InputSystem::SetMouseScreenCoords(Vector2& newMouseScreenCoords)
{
	int x = (int) newMouseScreenCoords.x;
	int y = (int) newMouseScreenCoords.y;
	SetCursorPos(x, y);
}


Vector2 InputSystem::GetScreenSize()
{
	HWND desktopWindowHandle = GetDesktopWindow();
	RECT desktopRect;
	GetWindowRect(desktopWindowHandle, &desktopRect);
	Vector2 screenSize;
	screenSize.x = (float)(desktopRect.right - desktopRect.left);
	screenSize.y = (float)(desktopRect.bottom - desktopRect.top);
	return screenSize;
}


void InputSystem::OnAppGainedFocus()
{
	m_doesAppHaveFocus = true;
	if (m_isMouseCursorHidden)
		HideMouseCursor();
}


void InputSystem::OnAppLostFocus()
{
	m_doesAppHaveFocus = false;
	if (!m_isMouseCursorHidden)
		ShowMouseCursor();
}


bool InputSystem::DoesAppHaveFocus()
{
	return m_doesAppHaveFocus;
}



void InputSystem::ShowMouseCursor()
{
	for (;;)
	{
		int newCursorShowValue = ShowCursor(TRUE);
		if (newCursorShowValue >= 0)
			break;
	}
}



void InputSystem::HideMouseCursor()
{
	for (;;)
	{
		int newCursorShowValue = ShowCursor(FALSE);
		if (newCursorShowValue < 0)
			break;
	}
}


void InputSystem::SetMouseCursorHiddenWhenWeAreFocused(bool hideMouseWhenFocused)
{
	m_isMouseCursorHidden = hideMouseWhenFocused;
	if (m_isMouseCursorHidden && m_doesAppHaveFocus)
		HideMouseCursor();
	else
		ShowMouseCursor();
}

void InputSystem::SetWheelScrollDelta(int newScrollDelta)
{
	m_wheelScrollDelta = newScrollDelta;
}
