#include "Engine/Profiling/Profiler.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Memory/Thread.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef PROFILED_BUILD
#include "Engine/Profiling/ProfilerReport.hpp"

ProfilerNode_t *g_activeProfilerNode = nullptr;
ProfilerNode_t *g_completedProfilerTree = nullptr;
bool g_isProfilerPaused = false;
bool g_willPauseNextFrame = false;
bool g_willResumeNextFrame = false;
bool g_isSnapshotting = false;
bool g_isTogglingView = false;
unsigned int g_numFramesPassed = 0;
double g_elapsedSeconds = 0.0;
DWORD g_mainThreadID = 0;

unsigned long long GetCurrentPerformanceCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (unsigned long long)li.QuadPart;
}

double PerformanceCounterToSeconds(unsigned long long counter)
{
	LARGE_INTEGER frequencyInteger;
	QueryPerformanceFrequency(&frequencyInteger);

	double frequencyQuadPart = double(frequencyInteger.QuadPart);

	return double(counter)/frequencyQuadPart;
}

void ProfilerSystemStartup()
{
	g_mainThreadID = ::GetCurrentThreadId();
}

void ProfilerSystemShutdown()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	ProfilerReport deletionReport(nullptr);

	if (g_activeProfilerNode != nullptr)
	{
		while(g_activeProfilerNode->parent != nullptr)
			ProfilerPop();
		ProfilerPop();
	}
	DeleteNode(g_completedProfilerTree);
}

void ProfilerPush(const char *tag)
{
	if (g_isProfilerPaused)
		return;

	ProfilerNode_t *newProfileNode = new ProfilerNode_t();
	newProfileNode->description = tag;
	newProfileNode->start = GetCurrentTimeSeconds();
	newProfileNode->end = 0;
	newProfileNode->parent = nullptr;
	newProfileNode->children = nullptr;
	newProfileNode->next = newProfileNode;
	newProfileNode->prev = newProfileNode;

	if (g_activeProfilerNode != nullptr)
	{
		newProfileNode->parent = g_activeProfilerNode;
		
		if (g_activeProfilerNode->children == nullptr)
		{
			g_activeProfilerNode->children = newProfileNode;
		}
		else
		{
			newProfileNode->next = g_activeProfilerNode->children;
			newProfileNode->prev = g_activeProfilerNode->children->prev;
			newProfileNode->next->prev = newProfileNode;
			newProfileNode->prev->next = newProfileNode;
		}
	}

	g_activeProfilerNode = newProfileNode;
}

void DeleteNode(ProfilerNode_t *node)
{
	if (node == nullptr)
		return;

	if (node->children != nullptr)
	{
		DeleteNode(node->children);
		node->children = nullptr;
	}

	ProfilerNode_t *headNode = node;
	ProfilerNode_t *currentNode = node->next;
	while (currentNode != headNode) 
	{
		if (currentNode->children != nullptr)
		{
			DeleteNode(currentNode->children);
			currentNode->children = nullptr;
		}

		ProfilerNode_t *nextNode = currentNode->next;

		currentNode->next = nullptr;
		currentNode->prev = nullptr;
		currentNode->parent = nullptr;
		currentNode->children = nullptr;
		delete currentNode;
		currentNode = nextNode;
	}

	node->next = nullptr;
	node->prev = nullptr;
	node->parent = nullptr;
	node->children = nullptr;
	delete node;
	node = nullptr;
}

void ProfilerPop()
{
	ASSERT_OR_DIE(g_activeProfilerNode != nullptr || g_isProfilerPaused || g_willResumeNextFrame, "A profiler node was popped off a non-existant tree.");

	if (g_isProfilerPaused)
		return;

	g_activeProfilerNode->end = GetCurrentTimeSeconds();

	if (g_activeProfilerNode->parent == nullptr)
	{
		if (g_completedProfilerTree != nullptr)
		{
			DeleteNode(g_completedProfilerTree);
			g_completedProfilerTree = nullptr;
		}

		g_completedProfilerTree = g_activeProfilerNode;
		g_activeProfilerNode = nullptr;

		if (g_willPauseNextFrame || g_isSnapshotting)
		{
			g_isProfilerPaused = true;
			g_isSnapshotting = false;
		}
	}
	else
	{
		g_activeProfilerNode = g_activeProfilerNode->parent;
	}
}

void ProfilerPause()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	g_willPauseNextFrame = true;
	g_willResumeNextFrame = false;
}

void ProfilerResume()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	g_willPauseNextFrame = false;
	g_willResumeNextFrame = true;
}

void ProfilerSnapshot()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	if (g_isProfilerPaused)
		g_isSnapshotting = true;
}

void ToggleTreeAndFlatView()
{
	g_isTogglingView = true;
}

ProfilerNode_t* ProfilerGetPreviousFrameNode()
{
	return g_completedProfilerTree;
}


ProfilerNode_t* ProfilerGetPreviousFrameWithTag(const char *tag)
{
	if (g_completedProfilerTree == nullptr)
		return nullptr;

	if (strcmp(tag, g_completedProfilerTree->description) == 0)
		return g_completedProfilerTree;

	return nullptr;
}

void PrintReport()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");

	g_numFramesPassed++;
	ProfilerNode_t *prevFrame = ProfilerGetPreviousFrameNode();
	if (prevFrame == nullptr)
		return;
	ProfilerReport report(prevFrame);

	if (g_isTogglingView)
	{
		g_isTreeView = !g_isTreeView;
		g_isTogglingView = false;
	}

	if (g_isTreeView)
	{
		report.CreateTreeView();
		report.SortByTotalTime();
	}
	else
	{
		report.CreateFlatView();
		report.SortBySelfTime();
	}
	
	g_elapsedSeconds += g_reportTree->totalTime;

	report.Log();

	if (g_isProfilerPaused && g_willResumeNextFrame)
		g_isProfilerPaused = false;

	if (g_isProfilerPaused && g_isSnapshotting)
		g_isProfilerPaused = false;
}

ProfileScope::ProfileScope(const char *tag)
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	if (!g_isProfilerPaused)
		ProfilerPush(tag);
}

ProfileScope::~ProfileScope()
{
	ASSERT_OR_DIE(g_mainThreadID == ::GetCurrentThreadId(), "Profiler function called from different thread.");
	ProfilerPop();
}

double GetTotalTime(ProfilerNode_t *node)
{
	if (node == nullptr)
		return 0.0;

	return node->end - node->start;
}

double GetChildrenTime(ProfilerNode_t *node)
{
	if (node->children == nullptr)
		return 0.f;

	double totalTime = 0.0;

	ProfilerNode_t *headNode = node->children;
	ProfilerNode_t *currentNode = node->children;

	do 
	{
		totalTime += GetTotalTime(currentNode);
		currentNode = currentNode->next;
	} while (currentNode != headNode);

	return totalTime;
}

double GetSelfTime(ProfilerNode_t *node)
{
	return GetTotalTime(node) - GetChildrenTime(node);
}

double GetTotalTimeForTagInSiblings(ProfilerNode_t *node, const char *tag)
{
	double totalTime = 0.0;

	ProfilerNode_t *currentNode = node;
	do
	{
		if (strcmp(tag, currentNode->description) == 0)
			totalTime += GetTotalTime(currentNode);
		
		currentNode = currentNode->children;
	} while (currentNode != nullptr);

	return totalTime;
}

double GetSelfTimeForTagInSiblings(ProfilerNode_t *node, const char *tag)
{
	double totalTime = 0.0;

	ProfilerNode_t *currentNode = node;
	do
	{
		if (strcmp(tag, currentNode->description) == 0)
			totalTime += GetSelfTime(currentNode);

		currentNode = currentNode->children;
	} while (currentNode != nullptr);

	return totalTime;
}

float GetFPS()
{
	return (float)g_numFramesPassed / (float)g_elapsedSeconds;
}

unsigned int GetNumCallsForTagInSiblings(ProfilerNode_t *node, const char *tag)
{
	unsigned int numCalls = 0;
	ProfilerNode_t *currentNode = node;

	do
	{
		if ( strcmp(tag, currentNode->description) == 0 )
			numCalls++;

		currentNode = currentNode->next;
	} while (currentNode != node);

	return numCalls;
}

unsigned int GetNumCallsForTagInChildren(ProfilerNode_t *node, const char *tag)
{
	unsigned int numCalls = 0;

	ProfilerNode_t *currentNode = node;
	do 
	{
		numCalls += GetNumCallsForTagInSiblings(currentNode, tag);
		currentNode = currentNode->children;
	} while (currentNode != nullptr);

	return numCalls;
}

#else

void ProfilerSystemStartup(){}
void ProfilerSystemShutdown(){}
void ProfilerPush(const char *tag) {tag;}
void ProfilerPop(){}
void PrintReport(){}

#endif
