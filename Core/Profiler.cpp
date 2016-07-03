#include "Profiler.h"
#include <fstream>
#include <sstream>

namespace FlagRTS
{
	Profiler* Profiler::_instance = 0;

	Profiler::Profiler()
	{

	}

	Profiler::~Profiler()
	{
		for(unsigned int i = 0; i < _clocks.size(); ++i)
			delete _clocks[i];
	}

	const std::string& Profiler::PrepareLog()
	{
		std::stringstream logBuild;
		logBuild.precision(9);
		logBuild<<"Profiler log:"<<std::endl;
		for(unsigned int i = 0; i < _clocks.size(); ++i)
		{
			logBuild<<std::endl;
			logBuild<<"Name: "<<_clocks[i]->ClockName<<std::endl;
			logBuild<<"Calls: "<<_clocks[i]->Calls<<std::endl;
			logBuild<<"Total time: "<<_clocks[i]->TotalTime * 1000.f<<std::endl;
		}
		_log = logBuild.str();

		return _log;
	}

	void Profiler::LogToFile(const char* fileName)
	{
		std::fstream file(fileName, std::ios::out | std::ios::trunc);
		PrepareLog();
		file<<_log;
		file.close();
	}

	ProfilerClock* Profiler::RegisterClock(const char* name)
	{
		auto clockIt = _clockNamesMap.find(std::string(name));
		if( clockIt != _clockNamesMap.end() )
			return clockIt->second;

		ProfilerClock* clock = new ProfilerClock(_clocks.size(), name);
		_clocks.push_back(clock);
		_clockNamesMap.insert(std::make_pair(
			std::string(name), clock));
		return clock;
	}
}