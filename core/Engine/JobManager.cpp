#include "stdafx.h"
#include "JobManager.h"
#include "Job.h"
#include "Profiler.h"

JobManager::JobManager()
{
	mJobCount = 0;
	shouldShutDown = false;
}

JobManager::~JobManager()
{

}

JobManager* JobManager::Get()
{
	static JobManager mJobManager;

	return &mJobManager;
}

void JobManager::Begin()
{
	for (size_t i = 0; i < MAX_NUM_WORKERS; i++)
	{
		mWorkers[i].Begin();
	}	
}

void JobManager::End()
{
	shouldShutDown = true;

	for (size_t i = 0; i < MAX_NUM_WORKERS; i++)
	{
		mWorkers[i].End();
	}
}

void JobManager::AddJob(Job* pJob)
{
	// Make sure pushing to the queue and job count increment happen atomically
	mLockForJobs.lock();

	mJobs.push(pJob);
	JobCountIncrement();

	mLockForJobs.unlock();
}

Job* JobManager::GetJob()
{
	// Acquire the lock before accessing the job queue
	mLockForJobs.lock();

	Job* availableJob = nullptr;

	if (mJobs.empty() == false)
	{
		availableJob = mJobs.front();
		mJobs.pop();
	}

	// Release the lock before returning the job to the worker
	mLockForJobs.unlock();

	return availableJob;
}

void JobManager::WaitForJobs()
{
	PROFILE_SCOPE(WaitForJobs);

	// Waiting for jobs to finish
	while (mJobCount > 0)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}