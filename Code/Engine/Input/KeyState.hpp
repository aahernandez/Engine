#pragma once

class KeyState
{
public:
	bool m_isDown;
	bool m_wasDown;

	KeyState();
	~KeyState();
};