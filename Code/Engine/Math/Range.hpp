#pragma once

class Range
{
public:
	float m_min;
	float m_max;

	Range();
	Range(float min, float max);
	~Range();

	float GetFloatInRange();
	bool IsFloatInRange(float floatToCheck);
};