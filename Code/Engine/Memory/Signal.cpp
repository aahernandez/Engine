#include "Signal.hpp"
#include "Thread.hpp"

static bool gSignalTestRunning = false;

Signal::Signal()
{
	os_event = ::CreateEvent(nullptr, TRUE, FALSE, NULL);
}

Signal::~Signal()
{
	::CloseHandle(os_event);
	os_event = NULL;
}

void Signal::SignalAll()
{
	::SetEvent(os_event);
}

void Signal::wait()
{
	DWORD result = ::WaitForSingleObject(os_event, INFINITE);
	if (result == WAIT_OBJECT_0) 
	{
		::ResetEvent(os_event);
	}
}

bool Signal::wait_for(unsigned int milliseconds)
{
	DWORD result = ::WaitForSingleObject(os_event, milliseconds);
	if (result == WAIT_OBJECT_0) 
	{
		::ResetEvent(os_event);
		return true;
	}

	return false;
}

struct SignalTest_t
{
	unsigned int index;
	Signal *signal;
};

static void SignalTestThread(void *data)
{
	SignalTest_t *test = (SignalTest_t*)data;

	unsigned int count = 0;
	while (gSignalTestRunning) 
	{
		test->signal->wait();
		++count;
		printf("Thread[%i] was signaled [%i].\n", test->index, count);
	}

	delete test;
}

void SignalTest()
{
	unsigned int num_threads = 8;
	Signal signal;

	gSignalTestRunning = true;

	for (unsigned int i = 0; i < num_threads; ++i) {
		SignalTest_t *test = new SignalTest_t();
		test->index = i;
		test->signal = &signal;

		ThreadHandle_t handle = ThreadCreate(SignalTestThread, test);
		ThreadDetach(handle);
	}

	ThreadSleep(50);

	for (unsigned int i = 0; i < 50; ++i) 
	{
		signal.SignalAll();
		ThreadSleep(100);
	}

	gSignalTestRunning = false;
	signal.SignalAll();

	ThreadSleep(100);
}