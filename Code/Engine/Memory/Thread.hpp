#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define INVALID_THREAD_HANDLE 0

typedef void* ThreadHandle_t;
typedef void(*JobWorkCB)(void*); 
ThreadHandle_t ThreadCreate(JobWorkCB cb, void *data);

void ThreadSleep(unsigned int ms);

void ThreadDetach(ThreadHandle_t th);
void ThreadJoin(ThreadHandle_t th);