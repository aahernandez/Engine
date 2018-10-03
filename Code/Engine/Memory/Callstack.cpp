#include "Engine/Memory/Callstack.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Memory/Memory.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Memory/LogSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma warning( disable : 4091 )
#include <DbgHelp.h>
#include <malloc.h>

#define MAX_SYMBOL_NAME_LENGTH 128
#define MAX_FILENAME_LENGTH 1024
#define MAX_DEPTH 128

typedef BOOL(__stdcall *sym_initialize_t)(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef BOOL(__stdcall *sym_cleanup_t)(IN HANDLE hProcess);
typedef BOOL(__stdcall *sym_from_addr_t)(IN HANDLE hProcess, IN DWORD64 Address, OUT PDWORD64 Displacement, OUT PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall *sym_get_line_t)(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Symbol);

static HMODULE g_DebugHelp;
static HANDLE g_Process;
static SYMBOL_INFO  *g_Symbol;

static sym_initialize_t LSymInitialize;
static sym_cleanup_t LSymCleanup;
static sym_from_addr_t LSymFromAddr;
static sym_get_line_t LSymGetLineFromAddr64;

static int g_CallstackCount = 0;

Callstack::Callstack()
	: m_hash(0)
	, m_frameCount(0)
{}

bool CallstackSystemInit()
{
	g_DebugHelp = LoadLibraryA("dbghelp.dll");
	if (g_DebugHelp == nullptr) 
		return false;

	LSymInitialize = (sym_initialize_t)GetProcAddress(g_DebugHelp, "SymInitialize");
	LSymCleanup = (sym_cleanup_t)GetProcAddress(g_DebugHelp, "SymCleanup");
	LSymFromAddr = (sym_from_addr_t)GetProcAddress(g_DebugHelp, "SymFromAddr");
	LSymGetLineFromAddr64 = (sym_get_line_t)GetProcAddress(g_DebugHelp, "SymGetLineFromAddr64");

	g_Process = ::GetCurrentProcess();
	LSymInitialize(g_Process, NULL, TRUE);

	g_Symbol = (SYMBOL_INFO *) ::malloc(sizeof(SYMBOL_INFO) + (MAX_FILENAME_LENGTH * sizeof(char)));
	g_Symbol->MaxNameLen = MAX_FILENAME_LENGTH;
	g_Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	return true;
}

void CallstackSystemDestroy()
{
	free(g_Symbol);
	g_Symbol = nullptr;

	LSymCleanup(g_Process);

	FreeLibrary(g_DebugHelp);
	g_DebugHelp = NULL;
}

Callstack* CreateCallstack(unsigned int skipFrames)
{
	void *stack[MAX_DEPTH];
	DWORD hash;

	unsigned int frames = CaptureStackBackTrace(1 + skipFrames, MAX_DEPTH, stack, &hash);
	Callstack *cs = (Callstack*) ::malloc(sizeof(Callstack));
	cs = new (cs) Callstack();

	unsigned int frameCount = min(MAX_FRAMES_PER_CALLSTACK, frames);
	cs->m_frameCount = frameCount;
	memcpy(cs->m_frames, stack, sizeof(void*) * frameCount);
	cs->m_hash = hash;

	return cs;
}

void DestroyCallstack(Callstack *c)
{
	::free( c );
}

unsigned int CallstackGetLines(CallstackLine_t *lineBuffer, unsigned int const maxLines, Callstack *cs)
{
	IMAGEHLP_LINE64 lineInfo;
	DWORD lineOffset = 0;
	lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);


	unsigned int count = min(maxLines, cs->m_frameCount);
	unsigned int idx = 0;

	for (unsigned int i = 0; i < count; ++i) 
	{
		CallstackLine_t *line = &(lineBuffer[idx]);
		DWORD64 ptr = (DWORD64)(cs->m_frames[i]);
		if (FALSE == LSymFromAddr(g_Process, ptr, 0, g_Symbol))
			continue;

		strcpy_s(line->functionName, 1024, g_Symbol->Name);
		BOOL bRet = LSymGetLineFromAddr64(GetCurrentProcess(), ptr, &lineOffset, &lineInfo);

		if (bRet) 
		{
			line->line = lineInfo.LineNumber;
			strcpy_s(line->filename, 128, lineInfo.FileName);
			line->offset = lineOffset;
		}
		else 
		{
			line->line = 0;
			line->offset = 0;
			strcpy_s(line->filename, 128, "N/A");
		}

		++idx;
	}

	return idx;
}

void PrintCallstack(AllocationNode_t *an)
{
	char bytesNumBuffer[128];
	sprintf_s(bytesNumBuffer, 128, "%u Bytes allocated at frame %u", an->allocation->byteSize, an->allocation->frame);
	g_theLogSystem->LogTaggedPrintf("memory", bytesNumBuffer);

	#if TRACK_MEMORY == TRACK_MEMORY_VERBOSE
		Callstack *cs = an->allocation->callstack;
		char lineBuffer[2048];
		CallstackLine_t lines[128];
		unsigned int lineCount = CallstackGetLines(lines, 128, cs);
		for (unsigned int i = 0; i < lineCount; ++i)
		{
			sprintf_s(lineBuffer, 2048, "%s(%u): %s", lines[i].filename, lines[i].line, lines[i].functionName);
			g_theLogSystem->LogPrint(lineBuffer);
		}
		g_theLogSystem->LogPrint("");
	#endif
}

char* GetCallstackString(Callstack *cs, char (&lineBuffer)[2048])
{
// 	lineBuffer;
// 	char lineBuffer2[2048];
	CallstackLine_t lines[128];
	unsigned int lineCount = CallstackGetLines(lines, 128, cs);
	for (unsigned int i = 0; i < lineCount; ++i)
	{
		sprintf_s(lineBuffer, 2048, "%s(%u): %s", lines[i].filename, lines[i].line, lines[i].functionName);
		g_theLogSystem->LogPrint(lineBuffer);
	}
	return lineBuffer;
// 	return lineBuffer2;
}
