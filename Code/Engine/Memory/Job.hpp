#pragma once
#if !defined( __JOB__ )
#define __JOB__

#include <vector>

typedef void(*JobWorkCB)(void*);

enum eJobType
{
	JOB_GENERIC,
	JOB_MAIN,
	JOB_LOGGING,
	JOB_ANALYTICS,
	JOB_IO,
	JOB_RENDER,

	NUM_JOB_TYPES,
};

class Job
{
public:
	eJobType m_type;
	JobWorkCB m_workCallback;
	void *m_userData;
	std::vector<Job*> m_dependents;
	unsigned int m_numDependencies;
	unsigned int m_referenceCount;

	void OnFinish();
	void OnDependancyFinished();
	void DependentOn(Job *parent);
};

void JobDispatchAndRelease(Job *job);

Job* JobCreate(eJobType type, JobWorkCB workCallback, void *userData);
void JobRun(eJobType type, JobWorkCB workCB, void *userData);
void JobDispatchAndRelease(Job *job);

#endif 
