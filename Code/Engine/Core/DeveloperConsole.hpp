#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Profiling/ProfilerReport.hpp"
#include <stdarg.h>
#include <map>
#include <vector>
#include <functional>

typedef void(*basic_event_cb)(void *user_arg, void *event_arg);

class DeveloperConsole
{
public:
	bool m_isConsoleActive;
	bool m_isProfilerActive;
	bool m_isMemoryProfilerActive;
	float m_caretTime;
	Vector2 m_projectionSize;
	std::string m_caret;
	std::string m_currentLine;
	Log m_log;
	float m_profilerXPos;
	float m_profilerYPos;
	std::map<std::string, std::function<void(std::string arg)> > m_commands;
	std::map<std::string, std::string > m_helpCommands;

	DeveloperConsole();
	~DeveloperConsole();

	void StoreTextInLog(std::string text, RGBA color = RGBA::WHITE);
	void StoreTextInLog(char const *text, RGBA color = RGBA::WHITE);
	void StoreTextInLog(bool boolPassed, RGBA color = RGBA::WHITE);
	void StoreTextInLog(char textChar, RGBA color = RGBA::WHITE);
	void StoreTaggedLog(std::string text);
	void ConsolePrintf(char const *format, ...);
	void ToggleConsole();
	void PushToLogQueue(ColoredText *newText);

	void InitLambdaFunctions();
	void UpdateCaret(float deltaSeconds);
	void Render();
	void RenderTranslucentOverlay();
	void RenderCaret();
	void RenderLog();
	void RenderMemoryProfiler();
	void RenderProfiler();
	void RenderProfileReportNode(ReportNode_t *node);
	void RecursivePrintProfiler(ReportNode_t *node);
	void LogMemoryProfiler();
	void AddCharacterToCurrentLine(int keyCode, bool isLowercase = true);
	void RemoveLastCharacterInCurrentLine();
	bool IsCurrentLineEmpty();
	void ClearCurrentLine();
	void AddFunctionCommand(char const *key, char const *description, std::function<void(std::string arg)> Func);
	void RunCommandFromCurrentLine();
	void RunCommand(std::string commandAndArgs);
	void EmptyLog();
	void DisplayHelp();
	void PasteFromWindowsClipboard();
	void HighlightWordInConsole(std::string word);
	void SetProjectionSize(const Vector2& projectionSize, const Vector2& windowSize);
	void ToggleMemoryProfiler();
	void ToggleProfiler();
	void SubscribeToEvent(void *userArg, basic_event_cb cb);
	void UnsubscribeToEvent(void *userArg, basic_event_cb cb);
	void Destroy();
};

void SetTagColor(char const *tag, const RGBA &color);