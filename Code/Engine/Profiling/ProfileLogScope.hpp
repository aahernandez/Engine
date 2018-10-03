#pragma once

#define PROFILE_LOG_SCOPE(desc) ProfileLogScope scope##__LINE__(desc)

class ProfileLogScope
{
public:
	float m_startTime;
	const char* m_description;

	ProfileLogScope(const char *description);
	~ProfileLogScope();
};