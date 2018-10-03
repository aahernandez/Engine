#pragma once

#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/Signal.hpp"
#include "Engine/Memory/Job.hpp"
#include "Engine/Memory/JobConsumer.hpp"

class JobSystem;
typedef void* ThreadHandle_t;
extern JobSystem *g_theJobSystem;

class JobSystem
{
public:
	ThreadSafeQueue<Job*> *m_queues;
	ThreadHandle_t *m_genericConsumerThreads;
	Signal **m_signals;
	JobConsumer *m_genericConsumer;
	JobConsumer *mainConsumer;
	JobConsumer *renderConsumer;
	unsigned int m_queueCount;
	int coreCount;

	bool m_isRunning;

	void SetTypeSignal(eJobType type, Signal *signal);
	void JobSystemMainStep();
	void JobSystemRenderStep();
};

void JobSystemStartup(int genericThreadCount = -1);
void JobSystemShutdown();
