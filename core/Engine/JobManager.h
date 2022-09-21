#pragma once
#include "Worker.h"
#include <queue>
#include <mutex>
#include <atomic>

#define MAX_NUM_WORKERS 4

class Job;

class JobManager
{
	public:
		static JobManager* Get();
		void Begin();
		void End();
		void AddJob(Job* pJob);
		Job* GetJob();
		void WaitForJobs();
		bool GetShutDownSignal() { return shouldShutDown; };
		void JobCountIncrement() { mJobCount++; };
		void JobCountDecrement() { mJobCount--; };

	private:
		JobManager();
		~JobManager();

		Worker mWorkers[MAX_NUM_WORKERS];	
		std::queue<Job*> mJobs;
		std::mutex mLockForJobs;
		std::atomic<int> mJobCount;
		bool shouldShutDown;
};

