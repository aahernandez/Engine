#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/RHI/Font.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Memory/Memory.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Memory/Thread.hpp"
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/Signal.hpp"
#include "Engine/Memory/Event.hpp"
#include "Engine/Profiling/Profiler.hpp"

ThreadSafeQueue<ColoredText*> g_DevMessages;
std::map<std::string, RGBA> g_tagColors;
ThreadHandle_t g_DevConThread = nullptr;
bool g_DevConThreadRunning = false;
Signal g_DevConSignal;
EventV0 g_DevConEvent;

void DevConsoleFlush()
{
	ColoredText *msg;
	while (g_DevMessages.Pop(&msg))
	{
		g_DevConEvent.trigger(msg);
	}
}

void StoreDataInLog(void *userArg, void *eventArg)
{
	userArg;
	ColoredText *msg = (ColoredText*)eventArg;
	g_theDevConsole->m_log.StoreTextInLog(msg->m_text, msg->m_color, DEFAULT, true);
// 	delete msg;
}

void DeveloperConsoleThread(void* data)
{
	data;
	g_DevConEvent.subscribe(nullptr, StoreDataInLog);

	while (g_DevConThreadRunning)
	{
		g_DevConSignal.wait();
		DevConsoleFlush();
	}
}

DeveloperConsole::DeveloperConsole()
	: m_isConsoleActive(false)
	, m_isProfilerActive(false)
	, m_isMemoryProfilerActive(false)
	, m_caretTime(0.f)
	, m_profilerXPos(0.f)
	, m_profilerYPos(0.f)
{

	ColoredText *newLog = new ColoredText();
	newLog->m_text = "";
	newLog->m_scale = m_log.m_scales[DEFAULT];
	newLog->m_color = RGBA::WHITE;

	m_log.m_coloredLog.insert(m_log.m_coloredLog.begin(), newLog);

	InitLambdaFunctions();

	g_DevConThread = ThreadCreate(DeveloperConsoleThread, nullptr);
	g_DevConThreadRunning = true;
}

DeveloperConsole::~DeveloperConsole()
{
	Destroy();
}


void DeveloperConsole::StoreTextInLog(char const *text, RGBA color /*= RGBA::WHITE*/)
{
	ColoredText *newLog = new ColoredText();
	newLog->m_text = text;
	newLog->m_color = color;
	newLog->m_scale = 1.f;
	
	PushToLogQueue(newLog);
}

void DeveloperConsole::StoreTextInLog(bool boolPassed, RGBA color /*= RGBA::WHITE*/)
{
	ColoredText *newLog = new ColoredText();
	if (boolPassed)
	{
		newLog->m_text = "true";
	}
	else
	{
		newLog->m_text = "false";
	}
	newLog->m_color = color;
	newLog->m_scale = 1.f;

	PushToLogQueue(newLog);
}

void DeveloperConsole::StoreTextInLog(std::string text, RGBA color /*= RGBA::WHITE*/)
{
	ColoredText *newLog = new ColoredText();
	newLog->m_text = text.c_str();
	newLog->m_color = color;
	newLog->m_scale = 1.f;

	PushToLogQueue(newLog);
}

void DeveloperConsole::StoreTextInLog(char textChar, RGBA color /*= RGBA::WHITE*/)
{
	std::string tempString = "";
	tempString += textChar;
	ColoredText *newLog = new ColoredText();
	newLog->m_text = tempString.c_str();
	newLog->m_color = color;
	newLog->m_scale = 1.f;

	PushToLogQueue(newLog);
}

void DeveloperConsole::StoreTaggedLog(std::string text)
{
	size_t found = text.find('[');
	std::string newLine = "";
	std::string logLineSecondHalf = text;
	std::string logLineFirstHalf;

	while (found != std::string::npos)
	{
		logLineFirstHalf = logLineSecondHalf.substr(0, found);
		logLineSecondHalf = logLineSecondHalf.substr(found + 1);
		if (logLineSecondHalf[0] >= '0' && logLineSecondHalf[0] <= '9')
		{
			newLine += '[';
			found = logLineSecondHalf.find('[');
			continue;
		}
		newLine += logLineFirstHalf;
		newLine += "[";
		found = logLineSecondHalf.find(']');
		logLineFirstHalf = logLineSecondHalf.substr(0, found);

		float tagColor[4] = {1.f, 1.f, 1.f, 1.f};
		if (g_tagColors.count(logLineFirstHalf) > 0)
			g_tagColors[logLineFirstHalf].GetAsFloats(tagColor[0], tagColor[1], tagColor[2], tagColor[3]);

		logLineSecondHalf = logLineSecondHalf.substr(found + 1);

		newLine += Stringf("{%f,%f,%f,%f}", tagColor[0], tagColor[1], tagColor[2], tagColor[3]);

		newLine += logLineFirstHalf;
		newLine += "{-}]";
		found = logLineSecondHalf.find('[');
	}
	newLine += logLineSecondHalf;
	newLine += " ";
	text = newLine;
	StoreTextInLog(text, RGBA::WHITE);
}

void DeveloperConsole::ConsolePrintf(char const *format, ...)
{
	va_list variableArgumentList;
	va_start(variableArgumentList, format);
	std::string text = Stringf(format, variableArgumentList);
	va_end(variableArgumentList);
	StoreTextInLog(text);
}

void DeveloperConsole::ToggleConsole()
{
	m_isConsoleActive = !m_isConsoleActive;
	if (m_isConsoleActive)
	{
		g_theInput->ShowMouseCursor();
	}
	else
	{
		g_theInput->HideMouseCursor();
	}
}

void DeveloperConsole::PushToLogQueue(ColoredText *newText)
{
	if (g_DevConThreadRunning == false)
		delete newText;
	else
	{
		g_DevMessages.Push(newText);
		g_DevConSignal.SignalAll();
	}
}

void DeveloperConsole::InitLambdaFunctions()
{
	std::function<void(std::string arg)> Func;
	Func = [&](std::string args) { EmptyLog(); };
	AddFunctionCommand("clear", "Clears the command console", Func);

	Func = [&](std::string args) { DisplayHelp(); };
	AddFunctionCommand("help", "Displays the known commands", Func);

	Func = [&](std::string args) { HighlightWordInConsole(args); };
	AddFunctionCommand("find", "Finds and highlights whatever argument is passed. Function form: find {0.3,0.9,0.9}word{-} ", Func);

	Func = [&](std::string args) { ToggleMemoryProfiler(); };
	AddFunctionCommand("profiler", "Toggles the profiler on or off.", Func);

	Func = [&](std::string args) { LogMemoryProfiler(); };
	AddFunctionCommand("getProfilerThisFrame", "Prints the current state of the basic memory profiler to the console.", Func);

	Func = [&](std::string args) 
	{
		std::vector<std::string> range = ParseStringIntoPiecesByDelimiter(args, ".");
		unsigned int min = (unsigned int)atoi(range[0].c_str());
		unsigned int max = (unsigned int)atoi(range[1].c_str());
		MemoryProfilerLogLiveAllocationsForFrame(min, max);
	};
	AddFunctionCommand("logAllocsInFrames", "Logs all allocations that happened within a frame range.", Func);
	
	#ifdef PROFILED_BUILD
		Func = [&](std::string args) { ProfilerPause(); };
		AddFunctionCommand("pauseProfiler", "Pauses the profiler after the current frame finishes.", Func);

		Func = [&](std::string args) { ProfilerResume(); };
		AddFunctionCommand("resumeProfiler", "Resumes the profiler.", Func);

		Func = [&](std::string args) { ProfilerSnapshot(); };
		AddFunctionCommand("snapProfiler", "Runs the profiler for a single frame and pauses.", Func);

		Func = [&](std::string args) { ToggleTreeAndFlatView(); };
		AddFunctionCommand("toggleProfilerView", "Switches back and forth between the profiler tree view and flat view.", Func);
	#endif
}

void DeveloperConsole::UpdateCaret(float deltaSeconds)
{
	m_caretTime += deltaSeconds;
	float cursorOnTime = SinDegrees(m_caretTime * 250.f);
	if (cursorOnTime > 0)
	{
		m_caret = "|";
	}
	else
	{
		m_caret = "";
	}
}

void DeveloperConsole::Render()
{
	RenderTranslucentOverlay();
	RenderCaret();
	RenderLog();
}

void DeveloperConsole::RenderTranslucentOverlay()
{
	g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteTexture);
	g_theSimpleRenderer->DrawQuad2D(Vector2(0.f, 0.f), m_projectionSize, Vector2(0.f, 0.f), Vector2(1.f, 1.f), RGBA(0.f, 0.f, 0.f, 0.6f));
}

void DeveloperConsole::RenderCaret()
{
	float length = g_theSimpleRenderer->m_font->CalculateTextWidth(m_currentLine.c_str());
	g_theSimpleRenderer->DrawText2D(m_caret.c_str(), Vector2(20.f + length, 50.f), RGBA::WHITE, 1.f, g_theSimpleRenderer->m_font);
}

void DeveloperConsole::RenderLog()
{
	m_log.RenderLog(Vector2(25.f, 50.f), true);
}

void DeveloperConsole::RenderMemoryProfiler()
{
	if (!m_isMemoryProfilerActive)
		return;

	g_theSimpleRenderer->DrawText2D("PROFILER", Vector2(20.f, 700.f), RGBA::WHITE, 1.f, g_theSimpleRenderer->m_font );
	
	#ifdef TRACK_MEMORY
		g_theSimpleRenderer->DrawText2D(Stringf("Bytes Allocated: %s", GetByteSizeAsString(MemoryProfilerGetTotalByteCount()).c_str()).c_str(), Vector2(40.f, 645.f), RGBA::WHITE, 0.8f);
		g_theSimpleRenderer->DrawText2D(Stringf("Highwater: %s", GetByteSizeAsString(MemoryProfilerGetHighwaterByteCount()).c_str()).c_str(), Vector2(40.f, 620.f), RGBA::WHITE, 0.8f);
		g_theSimpleRenderer->DrawText2D(Stringf("Total Allocations: %u", MemoryProfilerGetAllocationCount()).c_str(), Vector2(40.f, 595.f), RGBA::WHITE, 0.8f);
		g_theSimpleRenderer->DrawText2D(Stringf("Frame Allocations: %u", MemoryProfilerGetAllocationsLastFrame()).c_str(), Vector2(40.f, 570.f), RGBA::WHITE, 0.8f);
		g_theSimpleRenderer->DrawText2D(Stringf("Frame Frees: %u",		MemoryProfilerGetFreesLastFrame()).c_str(), Vector2(40.f, 545.f), RGBA::WHITE, 0.8f);

		#if TRACK_MEMORY == TRACK_MEMORY_BASIC
			g_theSimpleRenderer->DrawText2D("MEMORY {1,1,0}[BASIC]", Vector2(20.f, 670.f), RGBA::WHITE, 0.9f);
		#elif TRACK_MEMORY == TRACK_MEMORY_VERBOSE
			g_theSimpleRenderer->DrawText2D("MEMORY {1,1,0}[VERBOSE]", Vector2(20.f, 670.f), RGBA::WHITE, 0.9f);

		#endif

	#else
		g_theSimpleRenderer->DrawText2D("MEMORY {1,1,0}[DISABLED]", Vector2(20.f, 670.f), RGBA::WHITE, 0.9f);

	#endif // TRACK_MEMORY

	RenderProfiler();
}

void DeveloperConsole::RenderProfiler()
{
	if (!m_isProfilerActive)
		return;

	#ifdef PROFILED_BUILD
		m_profilerYPos = 375.f;

		if (g_reportTree != nullptr)
			g_theSimpleRenderer->DrawText2D(Stringf("Last Frame Time: %s", GetTimeAsString(g_reportTree->totalTime).c_str()).c_str(), Vector2(20.f,	m_profilerYPos + 100.f), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D(Stringf("FPS: %f", GetFPS()).c_str(), Vector2(20.f,	m_profilerYPos + 125.f), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);

		g_theSimpleRenderer->DrawText2D("FRAME TREE VIEW", Vector2(20.f,	m_profilerYPos + 50.f), RGBA::WHITE, 0.9f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("FUNCTION NAME", Vector2(20.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("CALLS", Vector2(450.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("% TOTAL", Vector2(550.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("(TIME)", Vector2(650.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("(AVG)", Vector2(750.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("% SELF", Vector2(900.f,	m_profilerYPos + 25.f), RGBA::WHITE, 0.8f,	g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("(TIME)", Vector2(1000.f, m_profilerYPos + 25.f), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
		g_theSimpleRenderer->DrawText2D("(AVG)", Vector2(1100.f, m_profilerYPos + 25.f), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);

		m_profilerXPos = 20.f;
		RecursivePrintProfiler(g_reportTree);

	#endif
}

void DeveloperConsole::RenderProfileReportNode(ReportNode_t *node)
{
	g_theSimpleRenderer->DrawText2D(Stringf("%s", node->tag).c_str(), Vector2(m_profilerXPos, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(Stringf("%i", node->numCalls).c_str(), Vector2(450.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(Stringf("%.2f%c", node->percentOfFrameTime * 100.f, '%').c_str(), Vector2(550.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(GetTimeAsString(node->totalTime).c_str(), Vector2(650.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(GetTimeAsString(node->averageTotalTime).c_str(), Vector2(750.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(Stringf("%.2f%c", node->percentOfSelfTime * 100.f, '%').c_str(), Vector2(900.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(GetTimeAsString(node->selfTime).c_str(), Vector2(1000.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
	g_theSimpleRenderer->DrawText2D(GetTimeAsString(node->averageSelfTime).c_str(), Vector2(1100.f, m_profilerYPos), RGBA::WHITE, 0.8f, g_theSimpleRenderer->m_font);
}

void DeveloperConsole::RecursivePrintProfiler(ReportNode_t *node)
{
	if (node == nullptr)
		return;

	#ifdef PROFILED_BUILD
		if (g_isTreeView)
		{
			RenderProfileReportNode(node);

			if (node->children != nullptr)
			{
				m_profilerXPos += 10.f;
				m_profilerYPos -= 25.f;
				RecursivePrintProfiler(node->children);
				m_profilerXPos -= 10.f;
			}

			ReportNode_t *currentNode = node->next;
			while (currentNode != node)
			{
				m_profilerYPos -= 25.f;
				RenderProfileReportNode(currentNode);
				if (currentNode->children != nullptr)
				{
					m_profilerXPos += 10.f;
					m_profilerYPos -= 25.f;
					RecursivePrintProfiler(currentNode->children);
					m_profilerXPos -= 10.f;
				}
				currentNode = currentNode->next;
			}
		}
		else
		{
			RenderProfileReportNode(node);

			if (node->children != nullptr)
			{
				m_profilerYPos -= 25.f;
				RecursivePrintProfiler(node->children);
			}
		}
	#endif

}

void DeveloperConsole::LogMemoryProfiler()
{
	#ifdef TRACK_MEMORY
		ConsolePrintf("Bytes Allocated : %u", MemoryProfilerGetTotalByteCount());
		ConsolePrintf("Highwater : %u", MemoryProfilerGetHighwaterByteCount());
		ConsolePrintf("Total Allocations : %u", MemoryProfilerGetAllocationCount());
		ConsolePrintf("Frame Allocations : %u", MemoryProfilerGetAllocationsLastFrame());
		ConsolePrintf("Frame Frees : %u", MemoryProfilerGetFreesLastFrame());
	#endif
}

void DeveloperConsole::AddCharacterToCurrentLine(int keyCode, bool isLowercase /*= true*/)
{
	char charCode;
	if (keyCode != KEY_TILDE && keyCode != 0
		&& keyCode != KEY_ESCAPE && keyCode != KEY_BACKSPACE
		&& keyCode != KEY_ENTER && keyCode != KEY_SHIFT
		&& keyCode != KEY_CTRL && keyCode != KEY_UPARROW
		&& keyCode != KEY_DOWNARROW && keyCode != KEY_DASH
		&& keyCode != KEY_PERIOD && keyCode != KEY_SLASH
		&& keyCode != KEY_COMMA && keyCode != KEY_SEMICOLON)
	{
		charCode = (char)keyCode;
	}
	else if (keyCode == KEY_DASH)
	{
		if (isLowercase)
		{
			charCode = '-';
		}
		else
		{
			charCode = '_';
		}
	}
	else if (keyCode == KEY_SEMICOLON)
	{
		if (isLowercase)
		{
			charCode = ';';
		}
		else
		{
			charCode = ':';
		}
	}
	else if (keyCode == KEY_PERIOD)
	{
		charCode = '.';
	}
	else if (keyCode == KEY_SLASH)
	{
		charCode = '/';
	}
	else if (keyCode == KEY_COMMA)
	{
		charCode = ',';
	}
	else
	{
		return;
	}

	char const character = (char const)charCode;
	char tempChar = character;
	if (isLowercase)
	{
		tempChar = (char)tolower(character);
	}
	m_currentLine += tempChar;

	ColoredText *newLog = new ColoredText();
	newLog->m_text = m_currentLine.c_str();
	newLog->m_scale = m_log.m_scales[DEFAULT];
	newLog->m_color = RGBA::WHITE;

	m_log.m_coloredLog[0] = newLog;
}

void DeveloperConsole::RemoveLastCharacterInCurrentLine()
{
	m_currentLine = m_currentLine.substr(0, m_currentLine.size() - 1);

	ColoredText *newLog = new ColoredText();
	newLog->m_text = m_currentLine.c_str();
	newLog->m_scale = m_log.m_scales[DEFAULT];
	newLog->m_color = RGBA::WHITE;

	m_log.m_coloredLog[0] = newLog;
}

bool DeveloperConsole::IsCurrentLineEmpty()
{
	return (m_currentLine.compare("") == 0);
}

void DeveloperConsole::ClearCurrentLine()
{
	(*m_log.m_coloredLog.begin())->m_text = "";
	m_currentLine = "";
}

void DeveloperConsole::AddFunctionCommand(char const *key, char const *description, std::function<void(std::string arg)> Func)
{
	m_commands[key] = Func;
	m_helpCommands[key] = "{0.9,0.3,0.9}";
	m_helpCommands[key] += key;
	m_helpCommands[key] += "{-} - ";
	m_helpCommands[key] += description;
}

void DeveloperConsole::RunCommandFromCurrentLine()
{

	size_t found = m_currentLine.find(" ");
	std::string commandLine = m_currentLine.substr(0, found);
	std::string args = m_currentLine.substr(found + 1);
	if (m_commands.find(commandLine) == m_commands.end()) {
		std::string invalidCommandString = "Invalid Command: ";
		invalidCommandString += m_currentLine;
		ConsolePrintf(invalidCommandString.c_str(), RGBA::RED);
	}
	else {
		ConsolePrintf(m_currentLine.c_str(), RGBA::GREEN_LIGHT);
		m_commands[commandLine](args);
	}
	ClearCurrentLine();
}

void DeveloperConsole::RunCommand(std::string commandAndArgs)
{
	std::vector<std::string> strings = ParseStringIntoPiecesByDelimiter(commandAndArgs, " ");
	if (strings.size() == 1)
	{
		m_commands[strings[0]]("");
	}
	else
	{
		m_commands[strings[0]](strings[1]);
	}
}

void DeveloperConsole::EmptyLog()
{
	ClearCurrentLine();
	m_log.EmptyLog();

	ColoredText *newLog = new ColoredText();
	newLog->m_text = "";
	newLog->m_color = RGBA::WHITE;

	m_log.m_coloredLog.insert(m_log.m_coloredLog.begin(), newLog);
}

void DeveloperConsole::DisplayHelp()
{
	ColoredText *text = new ColoredText();
	text->m_text = "Known Commands:";
	g_DevMessages.Push(text);
// 	ConsolePrintf("Known Commands:");

	std::map<std::string, std::string >::iterator iter = m_helpCommands.begin();
	for (iter; iter != m_helpCommands.end(); ++iter)
	{
		ColoredText *text2 = new ColoredText();
		text2->m_text = iter->second.c_str();
		g_DevMessages.Push(text2);
// 		g_DevMessages.Push(iter->second.c_str());
// 		ConsolePrintf(iter->second.c_str());
	}
	g_DevConSignal.SignalAll();
}

void DeveloperConsole::PasteFromWindowsClipboard()
{
	if (!OpenClipboard(NULL))
	{
		StoreTextInLog("Cannot open the clipboard", RGBA::RED);
		return;
	}

	HANDLE clip;
	clip = GetClipboardData(CF_TEXT);
	CloseClipboard();

	ConsolePrintf((char*)clip);
	ClearCurrentLine();
}

void DeveloperConsole::HighlightWordInConsole(std::string word)
{
	std::vector<ColoredText*>::iterator iter = m_log.m_coloredLog.begin();
	++iter;

	for (iter; iter != m_log.m_coloredLog.end(); ++iter)
	{
		size_t found = (*iter)->m_text.find(word);
		std::string newLine = "";
		std::string logLineSecondHalf = (*iter)->m_text;
		std::string logLineFirstHalf;

		while (found != std::string::npos)
		{
			logLineFirstHalf = logLineSecondHalf.substr(0, found);
			logLineSecondHalf = logLineSecondHalf.substr(found + word.size());
			newLine += logLineFirstHalf;
			newLine += "{1,1,0}";
			newLine += word;
			newLine += "{-}";
			found = logLineSecondHalf.find(word);
		}
		newLine += logLineSecondHalf;
		newLine += " ";
		(*iter)->m_text = newLine;
	}
}

void DeveloperConsole::SetProjectionSize(const Vector2& projectionSize, const Vector2& windowSize)
{
	m_projectionSize = projectionSize;
	m_log.m_windowScale = m_projectionSize.x / windowSize.x;
}

void DeveloperConsole::ToggleMemoryProfiler()
{
	m_isMemoryProfilerActive = !m_isMemoryProfilerActive;
}

void DeveloperConsole::ToggleProfiler()
{
	m_isProfilerActive = !m_isProfilerActive;
}

void DeveloperConsole::SubscribeToEvent(void *userArg, basic_event_cb cb)
{
	g_DevConEvent.subscribe(userArg, cb);
}

void DeveloperConsole::UnsubscribeToEvent(void *userArg, basic_event_cb cb)
{
	g_DevConEvent.unsubscribe(userArg, cb);
}

void DeveloperConsole::Destroy()
{
	g_DevConThreadRunning = false;
	g_DevConSignal.SignalAll();
	ThreadJoin(g_DevConThread);
	g_DevConThread = INVALID_THREAD_HANDLE;

	m_log.Destroy();
}

void SetTagColor(char const *tag, const RGBA &color)
{
	g_tagColors[tag] = color;
	std::string tagString = "[";
	tagString += tag;
	tagString += ']';
}
