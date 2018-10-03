#include "Engine/Time/Time.hpp"
#include "Engine/Time/Timer.hpp"

Timer::Timer(float frequency)
{
	SetFrequency(frequency);
}

Timer::~Timer()
{

}

void Timer::SetTimerSeconds(float seconds)
{
	m_intervalTime = seconds;
	m_targetTime = (float)GetCurrentTimeSeconds() + seconds;
}

void Timer::SetFrequency(float hz)
{
	SetTimerSeconds(1.f / hz);
}

bool Timer::Check()
{
	float currentTime = (float)GetCurrentTimeSeconds();
	return currentTime >= m_targetTime;
}

bool Timer::CheckAndDecrement()
{
	if (Check())
	{
		m_targetTime += m_intervalTime;
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::CheckAndReset()
{
	if (Check())
	{
		Reset();
		return true;
	}
	else
	{
		return false;
	}
}

unsigned int Timer::DecrementAll()
{
	unsigned int count = 0;
	while (CheckAndDecrement())
	{
		++count;
	}

	return count;
}

void Timer::Reset()
{
	m_targetTime = (float)GetCurrentTimeSeconds() + m_intervalTime;
}