#pragma once

#include "Engine/Memory/Job.hpp"

class JobConsumer
{
public:
	bool m_isUsingType[NUM_JOB_TYPES];

	JobConsumer();
	~JobConsumer();

	unsigned int ConsumeAll();
	unsigned int ConsumeAllForMS(unsigned int msToRun);
	bool ConsumeJob(Job * job);

	void AddType(eJobType newType);
};