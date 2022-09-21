#include "stdafx.h"
#include "Job.h"
#include "Worker.h"
#include "JobManager.h"

void Worker::Begin()
{
	mThread = std::thread(Loop);
}

void Worker::End()
{
	mThread.join();
}

void Worker::Loop()
{
	while (JobManager::Get()->GetShutDownSignal() != true)
	{
		// Sleep and wait for a new job
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));

		// Grab a new job if there is one
		Job* newJob = JobManager::Get()->GetJob();
		// Start the job
		if (newJob != nullptr)
		{
			newJob->DoIt();
		}
	}
}