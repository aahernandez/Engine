#include "Engine/Memory/LogSystem.hpp"
#include "Engine/Memory/Callstack.hpp"
#include "Engine/Memory/JobConsumer.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DeveloperConsole.hpp"

LogSystem *g_theLogSystem = nullptr;

void LogWriteToDebugger(void *user_arg, void *event_arg)
{
	user_arg;
	std::string *msg = (std::string*)event_arg;
	OutputDebugStringA(msg->c_str());
	OutputDebugStringA("\n");
}

void LogWriteToDevConsole(void *userArg, void *eventArg)
{
	userArg;
	std::string *msg = (std::string*)eventArg;
	g_theDevConsole->StoreTaggedLog(*msg);
}

void LogWriteToFile(void *user_arg, void *event_arg)
{
	user_arg;
	std::string *msg = (std::string*)event_arg;

	fprintf(g_theLogSystem->m_logFile, "%s\n", msg->c_str());

	LogWriteToDebugger(g_theLogSystem->m_logFile, event_arg);
	LogWriteToDevConsole(g_theLogSystem->m_logFile, event_arg);
}

void LoggerThread(void*)
{
	JobConsumer *logConsumer = new JobConsumer();
	logConsumer->AddType(JOB_LOGGING);
	g_theJobSystem->SetTypeSignal(JOB_LOGGING, &g_theLogSystem->m_logSignal);

	errno_t err = fopen_s(&g_theLogSystem->m_logFile, "Data/Logs/log.log", "w+");
	if ((err != 0) || (g_theLogSystem->m_logFile == nullptr))
	{
		return;
	}

	g_theLogSystem->m_logEvent.subscribe(nullptr, LogWriteToFile);

	while (g_theLogSystem->m_isLoggerThreadRunning)
	{
		g_theLogSystem->m_logSignal.wait();
		g_theLogSystem->LogFlush();

		logConsumer->ConsumeAll();
	}

	g_theLogSystem->LogFlush();
	fclose(g_theLogSystem->m_logFile);

	err = fopen_s(&g_theLogSystem->m_logFile, "Data/Logs/log.log", "r");
	if ((err != 0) || (g_theLogSystem->m_logFile == nullptr))
	{
		return;
	}

	FILE *outFile = nullptr;

	time_t t = time(0);
	struct tm * timeNow = localtime(&t);
	std::string outFileName = Stringf("Data/Logs/log_%i%i%i_%i%i%i.log", timeNow->tm_year + 1900, timeNow->tm_mon, timeNow->tm_mday, timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec);
	err = fopen_s(&outFile, outFileName.c_str(), "w+");
	if ((err != 0) || (g_theLogSystem->m_logFile == nullptr))
	{
		return;
	}

	const int size = 16384;
	char buffer[size];

	while (!feof(g_theLogSystem->m_logFile))
	{
		int n = fread(buffer, 1, size, g_theLogSystem->m_logFile);
		fwrite(buffer, 1, n, outFile);
	}

	fclose(outFile);
	fclose(g_theLogSystem->m_logFile);
}

void LogSystem::CloseCopyUseLogFile(char const *outFileName)
{
	g_theLogSystem->LogFlush();
	fclose(g_theLogSystem->m_logFile);
	g_theLogSystem->m_logFile = nullptr;

	FILE *readFile = nullptr;

	errno_t err = fopen_s(&readFile, "Data/Logs/log.log", "r");
	if ((err != 0) || (readFile == nullptr))
	{
		return;
	}

	err = fopen_s(&g_theLogSystem->m_logFile, outFileName, "a+");
	if ((err != 0) || (g_theLogSystem->m_logFile == nullptr))
	{
		return;
	}

	const int size = 16384;
	char buffer[size];

	while (!feof(readFile))
	{
		int n = fread(buffer, 1, size, readFile);
		fwrite(buffer, 1, n, g_theLogSystem->m_logFile);
	}

	fclose(readFile);
}

LogSystem::LogSystem()
	: m_areAllTagsDisabled(false)
	, m_isLoggerThreadRunning(true)
	, m_logFile(nullptr)
{
	m_loggerThread = ThreadCreate(LoggerThread, nullptr);
}

LogSystem::~LogSystem()
{
	g_theLogSystem->m_isLoggerThreadRunning = false;
	g_theLogSystem->m_logSignal.SignalAll();
	ThreadJoin(g_theLogSystem->m_loggerThread);
	g_theLogSystem->m_loggerThread = INVALID_THREAD_HANDLE;
}

void LogSystem::LogFlush()
{
	std::string msg;

	while (m_messages.Pop(&msg))
	{
		m_logEvent.trigger(&msg);
	}
}

void LogSystem::LogTaggedPrintf(char const *tag, char const *msg, ...)
{
	if (!IsTagAvailable(tag))
		return;

	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	time_t t = time(0);
	struct tm * timeNow = localtime(&t);

	LogPrint(Stringf("[%s] [%i/%i/%i %i:%i:%i] %s", tag, timeNow->tm_mon, timeNow->tm_mday, timeNow->tm_year + 1900, timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, textLiteral).c_str());
}

void LogSystem::LogWarningf(char const *msg, ...)
{
	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	LogTaggedPrintf("warning", textLiteral);
}

void LogSystem::LogErrorf(char const *msg, ...)
{
	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	LogTaggedPrintf("error", textLiteral);
	LogFlush();
// 	__debugbreak();
}

void LogSystem::LogPrintWithCallstackf(char const *tag, char const *msg, ...)
{
	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	Callstack *cs = CreateCallstack(1);
	char lineBuffer[2048];
	LogTaggedPrintf(tag, textLiteral);
	GetCallstackString(cs, lineBuffer);
}

void LogSystem::LogDisable(char const *tag)
{
	m_disabledTags.insert(tag);

	while (m_enabledTags.count(tag) > 0)
	{
		std::set<std::string>::iterator setIter = m_enabledTags.find(tag);
		m_enabledTags.erase(setIter);
	}
}

void LogSystem::LogEnable(char const *tag)
{
	m_enabledTags.insert(tag);

	while (m_disabledTags.count(tag) > 0)
	{
		std::set<std::string>::iterator setIter = m_disabledTags.find(tag);
		m_disabledTags.erase(setIter);
	}
}

void LogSystem::LogDisableAll()
{
	m_areAllTagsDisabled = true;
	m_enabledTags.clear();
}

void LogSystem::LogEnableAll()
{
	m_areAllTagsDisabled = false;
	m_disabledTags.clear();
}

bool LogSystem::IsTagAvailable(char const *tag)
{
	if (m_areAllTagsDisabled)
	{
		auto search = m_enabledTags.find(tag);
		if (search != m_enabledTags.end())
			return true;
	}
	else
	{
		auto search = m_disabledTags.find(tag);
		if (search == m_disabledTags.end())
			return true;
	}

	return false;
}

void LogSystem::LogPrint(char const *msg)
{
	m_messages.Push(msg);
	m_logSignal.SignalAll();
}

void LogSystemStartup()
{
	g_theLogSystem = new LogSystem();
}

void LogSystemShutdown()
{
	delete g_theLogSystem;
	g_theLogSystem = nullptr;
}

void LogSystem::LogPrintf(char const *msg, ...)
{
	char textLiteral[2048];
	va_list variableArgumentList;
	va_start(variableArgumentList, msg);
	vsnprintf_s(textLiteral, 2048, _TRUNCATE, msg, variableArgumentList);
	va_end(variableArgumentList);
	textLiteral[2048 - 1] = '\0';

	LogTaggedPrintf("log", textLiteral);
}

