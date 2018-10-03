#pragma once
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Memory/CriticalSection.hpp"

class Callstack;
extern CriticalSection g_memoryLock;

struct Allocation_t
{
	size_t byteSize;
	size_t frame;
#if TRACK_MEMORY == TRACK_MEMORY_VERBOSE
	Callstack *callstack;
#endif
};

struct AllocationNode_t
{
	Allocation_t* allocation;
	AllocationNode_t *nextNode;
	AllocationNode_t *prevNode;
};

void StartupMemoryProfiler();
void UpdateProfileMemory();

unsigned int MemoryProfilerGetTotalByteCount();
unsigned int MemoryProfilerGetAllocationCount();
unsigned int MemoryProfilerGetHighwaterByteCount();
unsigned int MemoryProfilerGetAllocationsLastFrame();
unsigned int MemoryProfilerGetFreesLastFrame();
void MemoryProfilerLogLiveAllocations();
void MemoryProfilerLogLiveAllocationsForFrame(unsigned int frameMin, unsigned int frameMax);

void AddToLinkedList(Allocation_t *newAlloc);
void RemoveFromLinkedList(Allocation_t *oldAlloc);