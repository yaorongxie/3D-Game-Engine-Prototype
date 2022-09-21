#include "stdafx.h"
#include "Profiler.h"
#include <stdio.h>
#include <thread>
#include <fstream>
#include <sstream>

Profiler::Profiler()
{
	fopen_s(&jsonFileStream, "profile.json", "w");

	if (jsonFileStream != nullptr)
	{
		fprintf(jsonFileStream, "[\n");
	}
}

Profiler::~Profiler()
{
	if (jsonFileStream != nullptr)
	{
		fclose(jsonFileStream);

		// Remove the ending comma in the last E event
		std::ifstream profileJsonIn("profile.json");
		std::stringstream buffer;
		buffer << profileJsonIn.rdbuf();
		std::string contents = buffer.str();
		profileJsonIn.close();
		contents.pop_back();
		contents.pop_back();

		// Add the ending ']' and overwrite the previous file
		contents += "\n]";
		std::ofstream profileJsonOut("profile.json");
		profileJsonOut << contents;
		profileJsonOut.close();
	}

	std::unordered_map<std::string, Timer*>::iterator it;

	FILE* txtFileStream;		
	fopen_s(&txtFileStream, "profile.txt", "w");

	if (txtFileStream != nullptr)
	{
		fprintf(txtFileStream, "name: avg (ms), max (ms)\n");

		for (it = mTimers.begin(); it != mTimers.end(); ++it)
		{
			fprintf(txtFileStream, "%s: %f, %f\n", it->second->mName.c_str(), it->second->GetAvg_ms(), it->second->GetMax_ms());
		}

		fclose(txtFileStream);
	}

	// Loop through all timers in the map and delete timers
	for (it = mTimers.begin(); it != mTimers.end(); ++it)
	{
		delete it->second;
	}
}

Profiler* Profiler::Get()
{
	static Profiler mProfiler;

	return &mProfiler;
}

void Profiler::BeginTimer(const std::string& name, uint64_t startTime)
{
	uint64_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
	
	fprintf(jsonFileStream, "{\"ph\": \"%s\", \"ts\": %llu, \"pid\": %llu, \"tid\": %llu, \"name\": \"%s\"},\n", 
			phB.c_str(), startTime, pid, tid, name.c_str());
}

void Profiler::EndTimer(uint64_t endTime)
{
	uint64_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
	
	fprintf(jsonFileStream, "{\"ph\": \"%s\", \"ts\": %llu, \"pid\": %llu, \"tid\": %llu},\n",
		phE.c_str(), endTime, pid, tid);
}

Profiler::Timer* Profiler::GetTimer(const std::string& name)
{
	std::unordered_map<std::string, Timer*>::iterator it = mTimers.find(name);

	// Timer exists in the map
	if (it != mTimers.end())
	{
		return mTimers[name];
	}
	// Timer does not exist in the map
	else
	{
		// Create a new timer with the given name
		Profiler::Timer* timer = new Profiler::Timer();
		timer->mName = name;
		// Add the new timer to the map
		mTimers[name] = timer;

		return timer;
	}
}

void Profiler::ResetAll()
{
	std::unordered_map<std::string, Timer*>::iterator it;

	// Loop through all timers in the map and call reset on them
	for (it = mTimers.begin(); it != mTimers.end(); ++it)
	{
		it->second->Reset();
	}
}

Profiler::Timer::Timer()
{
	mDurationInCurrentFrame = 0.0;
	mDurationInLongestFrame = 0.0;
	mTotalDuration = 0.0;
	mNumFrame = 0;
}

Profiler::Timer::~Timer()
{

}

void Profiler::Timer::Start()
{
	// Record the current time when the timer is started
	mStartTime = std::chrono::high_resolution_clock::now();

	Get()->BeginTimer(mName, mStartTime.time_since_epoch().count() / 1000);
}

void Profiler::Timer::Stop()
{
	// Record the current time when the timer is stopped
	mStopTime = std::chrono::high_resolution_clock::now();

	Get()->EndTimer(mStopTime.time_since_epoch().count() / 1000);

	// Accumulate to the total duration of this frame
	mDurationInCurrentFrame += (0.000001 * (double)std::chrono::duration_cast<std::chrono::nanoseconds>(mStopTime - mStartTime).count());
}

void Profiler::Timer::Reset()
{
	// Add the total duration for this frame to the overall total duration
	mTotalDuration += mDurationInCurrentFrame;

	// Increase the count of the number of frame
	mNumFrame++;

	// Update the longest frame time
	if (mDurationInCurrentFrame >= mDurationInLongestFrame)
	{
		mDurationInLongestFrame = mDurationInCurrentFrame;
	}

	// Reset the current frame¡¯s duration to 0
	mDurationInCurrentFrame = 0.0;
}

const std::string& Profiler::Timer::GetName() const
{
	return mName;
}

double Profiler::Timer::GetTime_ms() const
{
	return mDurationInCurrentFrame;
}

double Profiler::Timer::GetMax_ms() const
{
	return mDurationInLongestFrame;
}

double Profiler::Timer::GetAvg_ms() const
{
	return mTotalDuration / mNumFrame;
}


Profiler::ScopedTimer::ScopedTimer(Profiler::Timer* timer)
{
	mTimer = timer;
	mTimer->Start();
}

Profiler::ScopedTimer::~ScopedTimer()
{
	mTimer->Stop();
}