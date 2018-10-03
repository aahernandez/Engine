#pragma once

class Timer
{
public:
	float m_intervalTime;
	float m_targetTime;

	Timer(float frequency);
	~Timer();
	
	void SetTimerSeconds(float seconds);
	void SetFrequency(float hz);
	bool Check();
	bool CheckAndDecrement();
	bool CheckAndReset();
	unsigned int DecrementAll();
	void Reset();
};