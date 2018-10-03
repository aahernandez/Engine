#pragma once

#include "CriticalSection.hpp"
#include <condition_variable>

class Signal
{
public:
	Signal();
	~Signal();

	void SignalAll();
	void wait();
	bool wait_for(unsigned int ms);

public:
	HANDLE os_event;
};

void SignalTest();