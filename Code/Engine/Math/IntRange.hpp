#pragma once

class IntRange
{
public:
	int m_min;
	int m_max;

	IntRange();
	IntRange(int min, int max);
	~IntRange();

	int GetIntInRange();
	bool IsIntInRange(int testInt);
	bool IsBelowRange(int testInt);
	bool IsAboveRange(int testInt);
};