#include "Engine/Memory/Memory.hpp"

#ifdef TRACK_MEMORY

#include "Engine/Memory/Callstack.hpp"
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <malloc.h>
#include <map>
#include <string>
#include <random>
#pragma warning( disable : 4702 )

static unsigned int g_AllocCount = 0;
static unsigned int g_FrameAllocs = 0;
static unsigned int g_FrameFrees = 0;
static size_t g_AllocatedByteCount = 0;
static size_t g_AllocatedHighwaterByteCount = 0;
static size_t g_FramesPassed = 0;

static AllocationNode_t *head = nullptr;
CriticalSection g_memoryLock;

void* operator new(size_t const size)
 {
	++g_AllocCount;
	++g_FrameAllocs;
	g_AllocatedByteCount += size;
	if (g_AllocatedByteCount > g_AllocatedHighwaterByteCount)
		g_AllocatedHighwaterByteCount = g_AllocatedByteCount;
	
	size_t allocSize = size + sizeof(Allocation_t);
	Allocation_t *ptr = (Allocation_t*)malloc(allocSize);
	ptr->byteSize = size;
	ptr->frame = g_FramesPassed;
	#if TRACK_MEMORY == TRACK_MEMORY_VERBOSE
		SCOPE_LOCK(g_memoryLock);
		AddToLinkedList(ptr);
		Callstack *callStack = CreateCallstack(1);
		ptr->callstack = callStack;
	#endif

	return ptr + 1;
}

void operator delete(void *ptr)
{
	--g_AllocCount;
	++g_FrameFrees;
	
	Allocation_t *sizePtr = (Allocation_t*)ptr;
	sizePtr--; //subtracts sizeof(Allocation_t) from sizePtr so that only original memory is left. 
	#if TRACK_MEMORY == TRACK_MEMORY_VERBOSE
		SCOPE_LOCK(g_memoryLock);
		DestroyCallstack(sizePtr->callstack);
		RemoveFromLinkedList(sizePtr);
	#endif
	
	g_AllocatedByteCount -= sizePtr->byteSize;
	free(sizePtr);
}

void StartupMemoryProfiler()
{
	#ifdef TRACK_MEMORY
		g_memoryLock = CriticalSection();
	#endif
}

void UpdateProfileMemory()
{
	g_FrameAllocs = 0;
	g_FrameFrees = 0;
	g_FramesPassed++;
}

unsigned int MemoryProfilerGetTotalByteCount()
{
	return (unsigned int)g_AllocatedByteCount;
}

unsigned int MemoryProfilerGetAllocationCount()
{
	return g_AllocCount;
}

unsigned int MemoryProfilerGetHighwaterByteCount()
{
	return (unsigned int)g_AllocatedHighwaterByteCount;
}

unsigned int MemoryProfilerGetAllocationsLastFrame()
{
	return g_FrameAllocs;
}

unsigned int MemoryProfilerGetFreesLastFrame()
{
	return g_FrameFrees;
}

void MemoryProfilerLogLiveAllocations()
{
	if (head == nullptr)
		return;

	SCOPE_LOCK(g_memoryLock);

	AllocationNode_t *nodePtr = head;
	int linkedListSize = 0;
	do
	{
		linkedListSize++;
		nodePtr = nodePtr->nextNode;
	} while (nodePtr != head);

	nodePtr = head;
	AllocationNode_t *newHead = head;
	head = nullptr;

	for (int nodeCount = 0; nodeCount < linkedListSize; nodeCount++)
	{
		PrintCallstack(nodePtr);
		nodePtr = nodePtr->nextNode;
	}

	head = newHead;
}

void MemoryProfilerLogLiveAllocationsForFrame(unsigned int frameMin, unsigned int frameMax)
{
	if(head == nullptr)
		return;

	SCOPE_LOCK(g_memoryLock);
	AllocationNode_t *nodePtr = head;
	do
	{
		if (nodePtr->allocation->frame >= (size_t)frameMin && nodePtr->allocation->frame <= (size_t)frameMax)
		{
			PrintCallstack(nodePtr);
			nodePtr = nodePtr->nextNode;
		}
	} while (nodePtr != head);
}

void AddToLinkedList(Allocation_t *newAlloc)
{
	#if TRACK_MEMORY == TRACK_MEMORY_BASIC
		return;
	#endif

	AllocationNode_t *ptr = (AllocationNode_t*)malloc(sizeof(AllocationNode_t));
	ptr->allocation = newAlloc;

	if (head == nullptr)
	{
		ptr->nextNode = ptr;
		ptr->prevNode = ptr;
		head = ptr;
	}
	else
	{
		if (ptr->allocation->byteSize > head->allocation->byteSize)
		{
			ptr->nextNode = head;
			ptr->prevNode = head->prevNode;
			ptr->nextNode->prevNode = ptr;
			ptr->prevNode->nextNode = ptr;
			head = ptr;
			return;
		}

		AllocationNode_t *nodePtr = head;
		do
		{
			if (nodePtr->nextNode->allocation->byteSize < ptr->allocation->byteSize)
			{
				ptr->nextNode = nodePtr->nextNode;
				ptr->prevNode = nodePtr;
				ptr->nextNode->prevNode = ptr;
				ptr->prevNode->nextNode = ptr;
				return;
			}
			nodePtr = nodePtr->nextNode;
		} while(nodePtr != head);

		ptr->prevNode = head->prevNode;
		ptr->nextNode = head;
		head->prevNode->nextNode = ptr;
		head->prevNode = ptr;
	}
}

void RemoveFromLinkedList(Allocation_t *oldAlloc)
{
	#if TRACK_MEMORY == TRACK_MEMORY_BASIC
		return;
	#endif

	AllocationNode_t *ptr = head;
	do 
	{
		if (ptr->allocation == oldAlloc)
		{
			if (ptr == head)
			{
				if (ptr->nextNode != head && ptr->prevNode != head)
				{
					head = head->nextNode;
				}
				else
				{
					head = nullptr;
				}
			}

			ptr->prevNode->nextNode = ptr->nextNode;
			ptr->nextNode->prevNode = ptr->prevNode;
			ptr->nextNode = nullptr;
			ptr->prevNode = nullptr;

			free(ptr);

			break;
		}
		ptr = ptr->nextNode;
	} while (ptr != head);
}

#else

void MemoryProfilerLogLiveAllocations(){}
void MemoryProfilerLogLiveAllocationsForFrame(unsigned int, unsigned int){}
void UpdateProfileMemory(){}

#endif