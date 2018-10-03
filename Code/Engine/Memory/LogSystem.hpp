#pragma once
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/Thread.hpp"
#include "Engine/Memory/Signal.hpp"
#include "Engine/Memory/Event.hpp"

#include <set>

class LogSystem;
extern LogSystem *g_theLogSystem;

class LogSystem
{
public:
	std::set<std::string> m_disabledTags;
	std::set<std::string> m_enabledTags;
	bool m_areAllTagsDisabled;
	ThreadSafeQueue<std::string> m_messages;
	bool m_isLoggerThreadRunning;
	ThreadHandle_t m_loggerThread;
	Signal m_logSignal;
	EventV0 m_logEvent;
	FILE *m_logFile;
	
	LogSystem();
	~LogSystem();

	void LogPrint(char const *msg);
	void LogPrintf(char const *msg, ...);
	void LogFlush();
	void LogTaggedPrintf(char const *tag, char const *msg, ...);
	void LogWarningf(char const *msg, ...);
	void LogErrorf(char const *msg, ...);
	void LogPrintWithCallstackf(char const *tag, char const *msg, ...);
	void LogDisable(char const *tag);
	void LogEnable(char const *tag);
	void LogDisableAll();
	void LogEnableAll();
	bool IsTagAvailable(char const *tag);
	void CloseCopyUseLogFile(char const *outFileName);
};

void LogSystemStartup();
void LogSystemShutdown();