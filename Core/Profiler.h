#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <map>

// #define PROFILE_OFF

#ifndef PROFILE_OFF
#define PROFILE_ON
#endif

namespace FlagRTS
{
	struct ProfilerClock
	{
	public:
		LARGE_INTEGER ClockFreq;
		LARGE_INTEGER TicksOnStart;
		LARGE_INTEGER TicksOnEnd;
		int Id;
		int Calls;
		float TotalTime; // In seconds
		const char* ClockName;

		ProfilerClock(const int id, const char* name) : 
			Id(id),
			Calls(0),
			TotalTime(0.0f), 
			ClockName(name)
		{ }

		void Start()
		{
			++Calls;
			QueryPerformanceFrequency(&ClockFreq);
			QueryPerformanceCounter(&TicksOnStart);
		}

		void End()
		{
			QueryPerformanceCounter(&TicksOnEnd);
			TotalTime += static_cast<float>(TicksOnEnd.QuadPart - TicksOnStart.QuadPart)
				/ static_cast<float>(ClockFreq.QuadPart);
		}

		void Reset()
		{
			Calls = 0;
			TotalTime = 0.0f;
		}
	};

	class Profiler
	{
	private:
		static Profiler* _instance;
		std::vector<ProfilerClock*> _clocks;
		std::string _log;
		std::map<std::string, ProfilerClock*> _clockNamesMap;

		Profiler();

	public:
		~Profiler();
		static Profiler* GetProfiler()
		{
			if(_instance == 0)
			{
				_instance = new Profiler();
			}
			return _instance;
		}

		// Releases profiler instance ( one is created if GetProfiler() is called at least once )
		// All registered clocks are invalid after this call
		static void Release()
		{
			if(_instance != 0)
			{
				delete _instance;
				_instance = 0;
			}
		}

		const std::string& PrepareLog();
		void LogToFile(const char* fileName);

		ProfilerClock* RegisterClock(const char* name);
		ProfilerClock* GetClock(int id) { return _clocks[id]; }
		ProfilerClock* GetClock(const std::string& name) { return _clockNamesMap.find(name)->second; }
	};
}

#ifdef PROFILE_ON

#define PROFILE_REGISTER_CLOCK(clockname, blockname) \
	static FlagRTS::ProfilerClock* clockname = FlagRTS::Profiler::GetProfiler()->RegisterClock(blockname);

#define PROFILE_START(clockname) \
	clockname->Start();

#define PROFILE_START_NEW(clockname) \
	clockname->Reset(); \
	clockname->Start();

#define PROFILE_END(clockname) \
	clockname->End();

#define PROFILE_RESET_CLOCK(clockname) \
	clockname->Reset();

#define PROFILE_FIND_CLOCK(clockname, blockname) \
	static FlagRTS::ProfilerClock* clockname = FlagRTS::Profiler::GetProfiler()->GetClock(blockname);

#define PROFILE_LOG_TO_FILE(filename) \
	FlagRTS::Profiler::GetProfiler()->LogToFile(filename);

#define PROFILE_RELEASE() \
	FlagRTS::Profiler::Release();

#define PROFILE_GET_LOG() \
	FlagRTS::Profiler::GetProfiler()->PrepareLog();

#else
#define PROFILE_REGISTER_CLOCK(clockname, blockname)
#define PROFILE_START(clockname)
#define PROFILE_END(clockname)
#define PROFILE_FIND_CLOCK(cn, blockname)
#define PROFILE_RESET_CLOCK(clockname)
#define PROFILE_LOG_TO_FILE(filename) 
#define PROFILE_RELEASE()
#define PROFILE_GET_LOG()
#endif
