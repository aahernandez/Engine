#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/KeyState.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly

const float INNER_DEAD_ZONE = 8000.f;
const float OUTER_DEAD_ZONE = 23500;

enum XboxButton
{
	XBOX_DPAD_UP,
	XBOX_DPAD_DOWN,
	XBOX_DPAD_LEFT,
	XBOX_DPAD_RIGHT,
	XBOX_START,
	XBOX_BACK,
	XBOX_THUMB_LEFT,
	XBOX_THUMB_RIGHT,
	XBOX_BUMPER_LEFT,
	XBOX_BUMPER_RIGHT,
	XBOX_A,
	XBOX_B,
	XBOX_X,
	XBOX_Y,
	NUM_XBOX_BUTTONS
};

class XboxControl
{
	friend class InputSystem;

public:
	bool isButtonDown(XboxButton button);
	bool wasButtonJustPressed(XboxButton button);
	int GetLeftTrigger();
	int GetRightTrigger();
	Vector2 GetLeftThumb();
	Vector2 GetRightThumb();
	float GetLeftThumbPower();
	float GetRightThumbPower();

private:
	KeyState m_buttonStates[NUM_XBOX_BUTTONS];
	Vector2 leftThumb;
	Vector2 rightThumb;
	int controllerNumber;
	unsigned short xboxButtons;
	unsigned char leftTriggerPressure;
	unsigned char rightTriggerPressure;
	
	XboxControl();
	~XboxControl();

	unsigned long Update();
	void OnButtonDown(XboxButton button);
	void OnButtonUp(XboxButton button);

	void SetLeftThumbX(float);
	void SetLeftThumbY(float);
	void SetRightThumbX(float);
	void SetRightThumbY(float);

	float GetLeftThumbOrientationDegrees();
	float GetRightThumbOrientationDegrees();
};