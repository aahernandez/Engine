#include "Engine/Math/Range.hpp"
#include "Engine/Math/MathUtilities.hpp"

Range::Range()
	: m_min(0.f)
	, m_max(0.f)
{
}

Range::Range(float min, float max)
	: m_min(min)
	, m_max(max)
{
}

Range::~Range()
{

}

float Range::GetFloatInRange()
{
	return GetRandomFloatInRange(m_min, m_max);
}

bool Range::IsFloatInRange(float floatToCheck)
{
	if (floatToCheck >= m_min && floatToCheck <= m_max)
		return true;
	return false;
}
