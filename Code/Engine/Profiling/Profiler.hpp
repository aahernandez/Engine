#pragma once
#include "Engine/Core/BuildConfig.hpp"

#ifdef PROFILED_BUILD

#define PROFILE_SCOPE(tag) ProfileScope scope##__LINE__(tag)
#define PROFILE_SCOPE_FUNCTION PROFILE_SCOPE(__FUNCTION__)

struct ProfilerNode_t
{
	char const *description;
	double start;
	double end;

	ProfilerNode_t *parent;
	ProfilerNode_t *children;
	ProfilerNode_t *next;
	ProfilerNode_t *prev;
};

class ProfileScope
{
public:
	ProfileScope(const char *tag);
	~ProfileScope();
};

unsigned long long GetCurrentPerformanceCounter();
double PerformanceCounterToSeconds( unsigned long long counter );

void ProfilerSystemStartup();
void ProfilerSystemShutdown();
void ProfilerPush(const char *tag);
void ProfilerPop();
void ProfilerPause();
void ProfilerResume();
void ProfilerSnapshot();
void ToggleTreeAndFlatView();
void PrintReport();
void DeleteNode(ProfilerNode_t *node);
ProfilerNode_t* ProfilerGetPreviousFrameNode();
ProfilerNode_t* ProfilerGetPreviousFrameWithTag(const char *tag);
double GetChildrenTime(ProfilerNode_t *node);
double GetTotalTime(ProfilerNode_t *node);
double GetSelfTime(ProfilerNode_t *node);
float GetFPS();
double GetTotalTimeForTagInSiblings(ProfilerNode_t *node, const char *tag);
double GetSelfTimeForTagInSiblings(ProfilerNode_t *node, const char *tag);
unsigned int GetNumCallsForTagInChildren(ProfilerNode_t *node, const char *tag);
unsigned int GetNumCallsForTagInSiblings(ProfilerNode_t *node, const char *tag);

#else

#define PROFILE_SCOPE(tag)
#define PROFILE_SCOPE_FUNCTION PROFILE_SCOPE(__FUNCTION__)

void ProfilerSystemStartup();
void ProfilerSystemShutdown();
void PrintReport();

#endif