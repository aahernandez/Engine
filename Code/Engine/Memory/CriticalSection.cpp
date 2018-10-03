#include "Engine/Memory/CriticalSection.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

CriticalSection::CriticalSection()
{
	::InitializeCriticalSection(&criticalSection);
}

CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(&criticalSection);
}

void CriticalSection::Lock()
{
	::EnterCriticalSection(&criticalSection);
}

bool CriticalSection::TryLock()
{
	return TRUE == ::TryEnterCriticalSection(&criticalSection);
}

void CriticalSection::Unlock()
{
	::LeaveCriticalSection(&criticalSection);
}

ScopeCriticalSection::ScopeCriticalSection(CriticalSection *ptr)
{
	criticalSectionPtr = ptr;
	criticalSectionPtr->Lock();
}

ScopeCriticalSection::~ScopeCriticalSection()
{
	criticalSectionPtr->Unlock();
}