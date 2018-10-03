#pragma once

#include "Engine/Core/EngineCommon.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// creates a scoped critical section object named __lock_<LINE#>( &cs )
#define SCOPE_LOCK( criticalSection ) ScopeCriticalSection COMBINE(___lock_,__LINE__)(&criticalSection)

// Mutual Exclusive [mutex]: 
// Fair:  Serviced in order of request.
// Recursive: Someone is allowed to lock multiple times.
class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

	// Enters the lock - only one thread is allowed to be inside this lock at a time, every
	// other caller will block on this call until they're allowed to enter.
	void Lock();

	// Tries to enter the lock - returns TRUE if the lock was entered, false if it failed.
	bool TryLock();

	// Leave the lock - allowing the next person to lock to be able to enter.
	void Unlock();

public:
	CRITICAL_SECTION criticalSection;
};

class ScopeCriticalSection
{
public:
	ScopeCriticalSection(CriticalSection *ptr);
	~ScopeCriticalSection();

public:
	CriticalSection *criticalSectionPtr;
};