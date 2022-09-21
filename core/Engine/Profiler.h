#pragma once

#define PROFILE_SCOPE(name) \
Profiler::ScopedTimer name##_scope(Profiler::Get()->GetTimer(std::string(#name)))

class Profiler
{
	public:
		class Timer
		{
			public:
				friend Profiler;
				void Start();
				void Stop();
				void Reset();
				const std::string& GetName() const;
				double GetTime_ms() const;
				double GetMax_ms() const;
				double GetAvg_ms() const;

			private:
				Timer();
				~Timer();

				// Name to describe what this timer represents
				std::string mName;
				// How long this timer has taken in this frame ¨C in milliseconds
				double mDurationInCurrentFrame;
				// How long was this timer in the longest frame ¨C milliseconds
				double mDurationInLongestFrame;
				// The total time for this timer for all frames combined
				double mTotalDuration;
				// How many frames have been captured for this timer
				int mNumFrame;
				// Record the time when the timer is started
				std::chrono::high_resolution_clock::time_point mStartTime;
				// Record the time when the timer is stopped
				std::chrono::high_resolution_clock::time_point mStopTime;
		};

		class ScopedTimer
		{
			public:
				ScopedTimer(Timer* timer);
				~ScopedTimer();

			private:
				Timer* mTimer;
		};


		friend Timer;
		static Profiler* Get();
		Timer* GetTimer(const std::string& name);
		void ResetAll();

	private:
		Profiler();
		~Profiler();
		void BeginTimer(const std::string& name, uint64_t startTime);
		void EndTimer(uint64_t endTime);
		FILE* jsonFileStream;
		std::unordered_map<std::string, Timer*> mTimers;
		std::string phB = "B";
		std::string phE = "E";
		uint64_t pid = 1;
};

