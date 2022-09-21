#pragma once
#include <thread>

class Job;

class Worker
{
	public:
		void Begin();
		void End();
		static void Loop();

	private:	
		std::thread mThread;
};

