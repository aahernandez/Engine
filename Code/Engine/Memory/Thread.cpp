#include "Engine/Memory/Thread.hpp"

struct ThreadPassData_t
{
	JobWorkCB cb;
	void *arg;
};

static DWORD WINAPI ThreadEntryPointCommon(void *arg)
{
	ThreadPassData_t *passPtr = (ThreadPassData_t*)arg;

	passPtr->cb(passPtr->arg);
	delete passPtr;
	return 0;
}

ThreadHandle_t ThreadCreate(JobWorkCB cb, void *data)
{
	ThreadPassData_t *pass = new ThreadPassData_t();
	pass->cb = cb;
	pass->arg = data;

	DWORD threadID;
	ThreadHandle_t th = ::CreateThread(nullptr, 0, ThreadEntryPointCommon, pass, 0, &threadID);

	return th;
}

void ThreadSleep(unsigned int milleseconds)
{
	::Sleep(milleseconds);
}

void ThreadYield()
{
	::SwitchToThread();
}

void ThreadDetach(ThreadHandle_t th)
{
	::CloseHandle(th);
}

void ThreadJoin(ThreadHandle_t th)
{
	::WaitForSingleObject(th, INFINITE);
	::CloseHandle(th);
}