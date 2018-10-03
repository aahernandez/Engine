#include "Engine/Core/AnalyticsSystem.hpp"
#include "Engine/Memory/Callstack.hpp"
#include "Engine/Memory/JobConsumer.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Profiling/Profiler.hpp"

AnalyticsSystem *g_theAnalyticsSystem = nullptr;

void AnalyticsWriteToFile(void *user_arg, void *event_arg)
{
	user_arg;
	AnalyticsMessagePair *msgPair = (AnalyticsMessagePair*)event_arg;

	if (g_theAnalyticsSystem->m_analyticsFiles.find(msgPair->first) != g_theAnalyticsSystem->m_analyticsFiles.end())
	{
		fprintf(g_theAnalyticsSystem->m_analyticsFiles[msgPair->first], "%s\n", msgPair->second.c_str());
	}
	else
	{
		std::string errorMessage = Stringf("Filename %s does not exist.", msgPair->first.c_str());
		ASSERT_OR_DIE(false, errorMessage.c_str());
	}

}

void AnalyticsThread(void*)
{
	JobConsumer *analyticsConsumer = new JobConsumer();
	analyticsConsumer->AddType(JOB_ANALYTICS);
	g_theJobSystem->SetTypeSignal(JOB_ANALYTICS, &g_theAnalyticsSystem->m_analyticsSignal);

	g_theAnalyticsSystem->m_analyticsEvent.subscribe(nullptr, AnalyticsWriteToFile);

	while (g_theAnalyticsSystem->m_isAnalyticsThreadRunning)
	{
		g_theAnalyticsSystem->m_analyticsSignal.wait();
		g_theAnalyticsSystem->AnalyticsFlush();

		analyticsConsumer->ConsumeAll();
	}

	g_theAnalyticsSystem->AnalyticsFlush();

	std::map<std::string, FILE*>::iterator fileIter = g_theAnalyticsSystem->m_analyticsFiles.begin();
	while (fileIter != g_theAnalyticsSystem->m_analyticsFiles.end())
	{
		FILE* currentFile = fileIter->second;
		fclose(currentFile);
		free(currentFile);
		fileIter = g_theAnalyticsSystem->m_analyticsFiles.erase(fileIter);
	}
}

void AnalyticsSystem::CreateAnalyticsFile(char const *fileName)
{
	std::string name = fileName;
	std::string filePath = Stringf("Data/Analytics/%s.csv", fileName);
	g_theAnalyticsSystem->m_analyticsFiles[name] = nullptr;
	errno_t err = fopen_s(&g_theAnalyticsSystem->m_analyticsFiles[name], filePath.c_str(), "w+");
	if ((err != 0) || (g_theAnalyticsSystem->m_analyticsFiles[name] == nullptr))
	{
		return;
	}
}

AnalyticsSystem::AnalyticsSystem()
	: m_isAnalyticsThreadRunning(true)
{
	m_analyticsThread = ThreadCreate(AnalyticsThread, nullptr);
}

AnalyticsSystem::~AnalyticsSystem()
{
	g_theAnalyticsSystem->m_isAnalyticsThreadRunning = false;
	g_theAnalyticsSystem->m_analyticsSignal.SignalAll();
	ThreadJoin(g_theAnalyticsSystem->m_analyticsThread);
	g_theAnalyticsSystem->m_analyticsThread = INVALID_THREAD_HANDLE;
}

void AnalyticsSystem::AnalyticsFlush()
{
	AnalyticsMessagePair msg;

	while (m_messages.Pop(&msg))
	{
		m_analyticsEvent.trigger(&msg);
	}
}

void AnalyticsSystem::AnalyticsPrint(char const* fileName, char const* msg)
{
	m_messages.Push(std::make_pair(fileName, msg));
	m_analyticsSignal.SignalAll();
}

void AnalyticsSystemStartup()
{
	g_theAnalyticsSystem = new AnalyticsSystem();
}

void AnalyticsSystemShutdown()
{
	delete g_theAnalyticsSystem;
	g_theAnalyticsSystem = nullptr;
}

void AnalyticsSystem::AnalyticsPrintf(char const* fileName, char const *msg, ...)
{
	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	AnalyticsPrint(fileName, textLiteral);
}

