#pragma once
#include "Engine/Input/KeyState.hpp"
#include "Engine/Input/XboxControl.hpp"

const int NUM_KEYS = 256;
const int MAX_CONTROLLERS = 4;

enum KeyCode
{
	MOUSE_LEFT = 0x01,
	MOUSE_RIGHT = 0x02,
	KEY_BACKSPACE = 0x08,
	KEY_TAB = 0x09,
	KEY_ENTER = 0x0D,
	KEY_SHIFT = 0x10,
	KEY_CTRL = 0x11,
	KEY_ESCAPE = 0x1B,
	KEY_LEFTARROW = 0x25,
	KEY_UPARROW = 0x26,
	KEY_RIGHTARROW = 0x27,
	KEY_DOWNARROW = 0x28,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_F11 = 0x80,
	KEY_F12 = 0x81,
	KEY_SEMICOLON = 0xBA,
	KEY_PLUS = 0xBB,
	KEY_COMMA = 0xBC,
	KEY_DASH = 0xBD,
	KEY_MINUS = 0xBD,
	KEY_PERIOD = 0xBE,
	KEY_SLASH = 0xBF,
	KEY_TILDE = 0xC0,
	KEY_CARET = 0xDC,
};

class InputSystem
{
public:
	int lastKeyCode;
	bool m_doesAppHaveFocus;
	bool m_isMouseCursorHidden;
	int m_wheelScrollDelta;

	InputSystem();
	~InputSystem();

	void UpdateInputState();
	bool IsKeyDown(int keyCode);
	bool WasKeyJustPressed(int keyCode);
	void OnKeyDown(int keyCode);
	void OnKeyUp(int keyCode);
	int GetLastKeyCode();
	bool WasMouseJustScrolled();
	int PopScrollDelta();

	Vector2 GetMouseScreenCoords();
	Vector2 GetMouseWindowCoords();
	void SetMouseScreenCoords(Vector2& newMouseScreenCoords);
	Vector2 GetScreenSize();
	void OnAppGainedFocus();
	void OnAppLostFocus();
	bool DoesAppHaveFocus();
	void ShowMouseCursor();
	void HideMouseCursor();
	void SetMouseCursorHiddenWhenWeAreFocused(bool hideMouseWhenFocused);
	void SetWheelScrollDelta(int newScrollDelta);

	XboxControl& GetController(int controllerID);

private:
	KeyState m_keyStates[NUM_KEYS];
	XboxControl m_controllers[MAX_CONTROLLERS];

	void InitializeControllers();
	void UpdateKeyboard();
	void UpdateController();
	void RunMessagePump();
};

