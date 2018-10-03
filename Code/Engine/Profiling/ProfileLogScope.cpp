#include "Engine/Time/Time.hpp"
#include "Engine/Memory/LogSystem.hpp"
#include "Engine/Profiling/ProfileLogScope.hpp"

ProfileLogScope::ProfileLogScope(const char *description)
{
	m_startTime = (float)GetCurrentTimeSeconds();
	m_description = description;
}

ProfileLogScope::~ProfileLogScope()
{
	float totalTime = (float)GetCurrentTimeSeconds() - m_startTime;
	g_theLogSystem->LogTaggedPrintf("profiler", "'%s' took %f seconds.", m_description, totalTime);
}