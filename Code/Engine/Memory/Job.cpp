#include "Engine/Memory/Job.hpp"
#include "Engine/Memory/Atomic.hpp"
#include "Engine/Memory/Signal.hpp"
#include "Engine/Memory/ThreadSafeQueue.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Memory/JobConsumer.hpp"

void Job::OnFinish()
{
	for (unsigned int i = 0; i < m_dependents.size(); ++i) 
	{
		m_dependents[i]->OnDependancyFinished();
	}
}

void Job::OnDependancyFinished()
{
	JobDispatchAndRelease(this);
}

void Job::DependentOn(Job *parent)
{
	AtomicIncrement(&m_numDependencies);
	parent->m_dependents.push_back(this);
}

Job* JobCreate(eJobType type, JobWorkCB work_cb, void *user_data)
{
	Job *job = new Job();
	job->m_type = type;
	job->m_workCallback = work_cb;
	job->m_userData = user_data;
	job->m_numDependencies = 1;

	return job;
}

void JobDispatchAndRelease(Job *job)
{
	unsigned int dcount = AtomicDecrement(&job->m_numDependencies);
	if (dcount != 0) 
	{
		return;
	}

	g_theJobSystem->m_queues[job->m_type].Push(job);
	Signal *signal = g_theJobSystem->m_signals[job->m_type];
	if (nullptr != signal) 
	{
		signal->SignalAll();
	}
}

void JobRun(eJobType type, JobWorkCB workCB, void *userData)
{
	Job *job = JobCreate(type, workCB, userData);
	JobDispatchAndRelease(job);
}
