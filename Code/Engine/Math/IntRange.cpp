#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/MathUtilities.hpp"

IntRange::IntRange()
	: m_min(0)
	, m_max(0)
{
}

IntRange::IntRange(int min, int max)
	: m_min(min)
	, m_max(max)
{

}

IntRange::~IntRange()
{

}

int IntRange::GetIntInRange()
{
	return GetRandomIntInRange(m_min, m_max);
}

bool IntRange::IsIntInRange(int testInt)
{
	return m_min <= testInt && m_max >= testInt;
}

bool IntRange::IsBelowRange(int testInt)
{
	return testInt < m_min;
}

bool IntRange::IsAboveRange(int testInt)
{
	return testInt > m_max;
}

