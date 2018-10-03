#include "Engine/Input/XboxControl.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> // include the Xinput API

XboxControl::XboxControl()
	: xboxButtons(0)
	, controllerNumber(0)
	, leftTriggerPressure(NULL)
	, rightTriggerPressure(NULL)
	, leftThumb(0.f,0.f)
	, rightThumb(0.f,0.f)
{
}

XboxControl::~XboxControl()
{

}

unsigned long XboxControl::Update()
{
	for (int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++buttonIndex)
	{
		KeyState& keyState = m_buttonStates[buttonIndex];
		keyState.m_wasDown = keyState.m_isDown;
	}

	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	unsigned long errorStatus = XInputGetState( controllerNumber, &xboxControllerState );
	if (errorStatus == ERROR_SUCCESS)
	{
		xboxButtons = xboxControllerState.Gamepad.wButtons;

		m_buttonStates[XBOX_DPAD_UP].m_isDown = (xboxButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		m_buttonStates[XBOX_DPAD_DOWN].m_isDown = (xboxButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		m_buttonStates[XBOX_DPAD_LEFT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		m_buttonStates[XBOX_DPAD_RIGHT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
		m_buttonStates[XBOX_START].m_isDown = (xboxButtons & XINPUT_GAMEPAD_START) != 0;
		m_buttonStates[XBOX_BACK].m_isDown = (xboxButtons & XINPUT_GAMEPAD_BACK) != 0;
		m_buttonStates[XBOX_THUMB_LEFT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
		m_buttonStates[XBOX_THUMB_RIGHT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
		m_buttonStates[XBOX_BUMPER_LEFT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		m_buttonStates[XBOX_BUMPER_RIGHT].m_isDown = (xboxButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
		m_buttonStates[XBOX_A].m_isDown = (xboxButtons & XINPUT_GAMEPAD_A) != 0;
		m_buttonStates[XBOX_B].m_isDown = (xboxButtons & XINPUT_GAMEPAD_B) != 0;
		m_buttonStates[XBOX_X].m_isDown = (xboxButtons & XINPUT_GAMEPAD_X) != 0;
		m_buttonStates[XBOX_Y].m_isDown = (xboxButtons & XINPUT_GAMEPAD_Y) != 0;

		leftTriggerPressure = xboxControllerState.Gamepad.bLeftTrigger;
		rightTriggerPressure = xboxControllerState.Gamepad.bRightTrigger;
	
		SetLeftThumbX( (float) xboxControllerState.Gamepad.sThumbLX );
		SetLeftThumbY( (float) xboxControllerState.Gamepad.sThumbLY );
		SetRightThumbX( (float) xboxControllerState.Gamepad.sThumbRX );
		SetRightThumbY( (float) xboxControllerState.Gamepad.sThumbRY );
	}
	return errorStatus;
}

bool XboxControl::isButtonDown(XboxButton button)
{
	return m_buttonStates[button].m_isDown;
}

bool XboxControl::wasButtonJustPressed(XboxButton button)
{
	bool keyWasDown = m_buttonStates[button].m_wasDown;
	bool keyIsDown = m_buttonStates[button].m_isDown;
	return keyIsDown && !keyWasDown;
}

int XboxControl::GetLeftTrigger()
{
	return leftTriggerPressure;
}

int XboxControl::GetRightTrigger()
{
	return rightTriggerPressure;
}

void XboxControl::OnButtonDown(XboxButton button)
{
	m_buttonStates[button].m_isDown = true;
}

void XboxControl::OnButtonUp(XboxButton button)
{
	m_buttonStates[button].m_isDown = false;
}

void XboxControl::SetLeftThumbX(float leftThumbX)
{
	leftThumb.x = leftThumbX;
}

void XboxControl::SetLeftThumbY(float leftThumbY)
{
	leftThumb.y = leftThumbY;
}

void XboxControl::SetRightThumbX(float rightThumbX)
{
	rightThumb.x = rightThumbX;
}

void XboxControl::SetRightThumbY(float rightThumbY)
{
	rightThumb.y = rightThumbY;
}

float XboxControl::GetLeftThumbPower()
{
	float leftThumbThrust = leftThumb.CalcLength();
// 	leftThumbThrust = RangeMap1D(leftThumbThrust, INNER_DEAD_ZONE, OUTER_DEAD_ZONE, 0.f, 1.f);
	leftThumbThrust = fabs(leftThumbThrust);

	if (leftThumbThrust > 1.f)
		leftThumbThrust = 1.f;
	if (leftThumbThrust < 0.f)
		leftThumbThrust = 0.f;

	return leftThumbThrust;
}


float XboxControl::GetLeftThumbOrientationDegrees()
{
	return leftThumb.CalcHeadingDegrees();
}

float XboxControl::GetRightThumbPower()
{
	float rightThumbThrust = rightThumb.CalcLength();
	rightThumbThrust = RangeMap1D(rightThumbThrust, INNER_DEAD_ZONE, OUTER_DEAD_ZONE, 0.f, 1.f);

	if (rightThumbThrust > 1.f)
		rightThumbThrust = 1.f;
	if (rightThumbThrust < 0.f)
		rightThumbThrust = 0.f;

	return rightThumbThrust;
}

float XboxControl::GetRightThumbOrientationDegrees()
{
	return rightThumb.CalcHeadingDegrees();
}

Vector2 XboxControl::GetLeftThumb()
{
	float leftThumbPower = leftThumb.CalcLength();
	leftThumbPower = RangeMap1D(leftThumbPower, INNER_DEAD_ZONE, OUTER_DEAD_ZONE, 0.f, 1.f);

	if (leftThumbPower > 1.f)
		leftThumbPower = 1.f;
	if (leftThumbPower < 0.f)
		leftThumbPower = 0.f;

	float leftThumbOrientation = GetLeftThumbOrientationDegrees();
	leftThumb.SetLengthAndHeadingDegrees(leftThumbPower, leftThumbOrientation);
	return leftThumb;
}

Vector2 XboxControl::GetRightThumb()
{
	float rightThumbPower = GetRightThumbPower();
	float rightThumbOrientation = GetRightThumbOrientationDegrees();
	rightThumb.SetLengthAndHeadingDegrees(rightThumbPower, rightThumbOrientation);
	return rightThumb;
}
