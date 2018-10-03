#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Memory/Thread.hpp"

JobSystem *g_theJobSystem = nullptr;

static void GenericJobThread(void *voidSignal)
{
	voidSignal;
	JobConsumer *genericConsumer = g_theJobSystem->m_genericConsumer;
	Signal *signal = g_theJobSystem->m_signals[JOB_GENERIC];

	while (g_theJobSystem->m_isRunning)
	{
		signal->wait();
		genericConsumer->ConsumeAll();
	}

	genericConsumer->ConsumeAll();
}

void JobSystemStartup(int genericThreadCount /*= -1*/)
{
	g_theJobSystem = new JobSystem();

	unsigned int jobCategoryCount = NUM_JOB_TYPES;
	g_theJobSystem->coreCount = (int)std::thread::hardware_concurrency();
	if (genericThreadCount <= 0)
	{
		g_theJobSystem->coreCount += genericThreadCount;
	}
	g_theJobSystem->coreCount--;

	g_theJobSystem->m_queues = new ThreadSafeQueue<Job*>[jobCategoryCount];
	g_theJobSystem->m_signals = new Signal*[jobCategoryCount];
	g_theJobSystem->m_genericConsumerThreads = new ThreadHandle_t[g_theJobSystem->coreCount + 1];
	g_theJobSystem->m_genericConsumer = new JobConsumer();
	g_theJobSystem->m_genericConsumer->AddType(JOB_GENERIC);
	g_theJobSystem->mainConsumer = new JobConsumer();
	g_theJobSystem->mainConsumer->AddType(JOB_MAIN);
	g_theJobSystem->renderConsumer = new JobConsumer();
	g_theJobSystem->renderConsumer->AddType(JOB_RENDER);
	g_theJobSystem->m_queueCount = jobCategoryCount;
	g_theJobSystem->m_isRunning = true;

	for (unsigned int i = 0; i < jobCategoryCount; ++i)
	{
		g_theJobSystem->m_signals[i] = nullptr;
	}

	g_theJobSystem->m_signals[JOB_GENERIC] = new Signal();

	g_theJobSystem->m_genericConsumerThreads[0] = ThreadCreate(GenericJobThread, nullptr);
	for (int i = 0; i < g_theJobSystem->coreCount; ++i)
	{
		g_theJobSystem->m_genericConsumerThreads[i + 1] = ThreadCreate(GenericJobThread, nullptr);
	}
}

void JobSystemShutdown()
{
	for (int i = 0; i < g_theJobSystem->coreCount + 1; ++i)
	{
		g_theJobSystem->m_isRunning = false;
		g_theJobSystem->m_signals[JOB_GENERIC]->SignalAll();
		ThreadJoin(g_theJobSystem->m_genericConsumerThreads[i]);
		g_theJobSystem->m_genericConsumerThreads[i] = INVALID_THREAD_HANDLE;
	}

	delete g_theJobSystem->m_signals[JOB_GENERIC];
	g_theJobSystem->m_signals[JOB_GENERIC] = nullptr;

	for (int currentJobCount = 0; currentJobCount < NUM_JOB_TYPES; currentJobCount++)
	{
		Job *job;
		while(g_theJobSystem->m_queues[currentJobCount].Pop(&job))
		{
			delete job;
			job = nullptr;
		}
	}

	delete g_theJobSystem;
	g_theJobSystem = nullptr;
}

void JobSystem::SetTypeSignal(eJobType type, Signal *signal)
{
	m_signals[type] = signal;
}

void JobSystem::JobSystemMainStep()
{
	mainConsumer->ConsumeAll();
}

void JobSystem::JobSystemRenderStep()
{
	renderConsumer->ConsumeAll();
}
