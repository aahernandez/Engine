#include "Engine/Memory/JobConsumer.hpp"
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Time/Time.hpp"

JobConsumer::JobConsumer()
{
	for (unsigned int typeIndex = 0; typeIndex < (unsigned int)NUM_JOB_TYPES; typeIndex++)
	{
		m_isUsingType[typeIndex] = false;
	}
}

JobConsumer::~JobConsumer()
{
}

unsigned int JobConsumer::ConsumeAll()
{
	Job *job;
	unsigned int processed_jobs = 0;

	for (unsigned int typeIndex = 0; typeIndex < (unsigned int)NUM_JOB_TYPES; typeIndex++)
	{
		if (m_isUsingType[typeIndex])
		{
			ThreadSafeQueue<Job*> &queue = g_theJobSystem->m_queues[typeIndex];
			while (queue.Pop(&job)) 
			{
				job->m_workCallback(job->m_userData);
				++processed_jobs;

				job->OnFinish();
				delete job;
			}
		}
	}


	return processed_jobs;
}

unsigned int JobConsumer::ConsumeAllForMS(unsigned int msToRun)
{
	Job *job;
	unsigned int processed_jobs = 0;
	float startTime = (float)GetCurrentTimeSeconds();
	float secondsToRun = (float)msToRun / 1000.f;
	float runningTime = 0.f;

	for (unsigned int typeIndex = 0; typeIndex < (unsigned int)NUM_JOB_TYPES; typeIndex++)
	{
		if (m_isUsingType[typeIndex])
		{
			ThreadSafeQueue<Job*> &queue = g_theJobSystem->m_queues[typeIndex];
			while (queue.Pop(&job))
			{
				job->m_workCallback(job->m_userData);
				++processed_jobs;

				job->OnFinish();
				delete job;

				runningTime = (float)GetCurrentTimeSeconds() - startTime;
				if (runningTime >= secondsToRun)
					return processed_jobs;
			}
		}
	}

	return processed_jobs;
}

bool JobConsumer::ConsumeJob(Job *job)
{
	if (m_isUsingType[job->m_type])
	{
		job->m_workCallback(job->m_userData);
		job->OnFinish();
		delete job;
		return true;
	}

	return false;
}

void JobConsumer::AddType(eJobType newType)
{
	m_isUsingType[newType] = true;
}
