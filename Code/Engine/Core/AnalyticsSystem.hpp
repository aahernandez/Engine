#pragma once
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/Thread.hpp"
#include "Engine/Memory/Signal.hpp"
#include "Engine/Memory/Event.hpp"

#include <map>
#include <utility>

class AnalyticsSystem;
extern AnalyticsSystem *g_theAnalyticsSystem;

typedef std::pair<std::string, std::string> AnalyticsMessagePair; //analytics file, message

class AnalyticsSystem
{
public:
	ThreadSafeQueue<AnalyticsMessagePair> m_messages;
	bool m_isAnalyticsThreadRunning;
	ThreadHandle_t m_analyticsThread;
	Signal m_analyticsSignal;
	EventV0 m_analyticsEvent;
	std::map<std::string, FILE*> m_analyticsFiles;

	AnalyticsSystem();
	~AnalyticsSystem();

	void AnalyticsPrint(char const* fileName, char const *msg);
	void AnalyticsPrintf(char const* fileName, char const *msg, ...);
	void AnalyticsFlush();
	void CreateAnalyticsFile(char const *fileName);
};

void AnalyticsSystemStartup();
void AnalyticsSystemShutdown();