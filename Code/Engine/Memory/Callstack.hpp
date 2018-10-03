#pragma once

struct AllocationNode_t;

#define MAX_FRAMES_PER_CALLSTACK (128)

struct CallstackLine_t
{
	char filename[128];
	char functionName[1024];
	unsigned int line;
	unsigned int offset;
};

class Callstack
{
public:
	unsigned int m_hash;
	unsigned int m_frameCount;
	void* m_frames[MAX_FRAMES_PER_CALLSTACK];

	Callstack();
};

bool CallstackSystemInit();
void CallstackSystemDestroy();

Callstack* CreateCallstack(unsigned int skipFrames);
void DestroyCallstack(Callstack *c);
unsigned int CallstackGetLines(CallstackLine_t *lineBuffer, unsigned int const max_lines, Callstack *cs);
void PrintCallstack(AllocationNode_t *an);
char* GetCallstackString(Callstack *cs, char (&lineBuffer)[2048]);