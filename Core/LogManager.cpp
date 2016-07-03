#include "LogManager.h"
#include <Windows.h>
#include "Memory.h"

namespace FlagRTS
{
	LogManager* LogManager::_instance = 0;

	LogManager::LogManager()
	{

	}

	LogManager::~LogManager()
	{
		for(unsigned int i = 0; i < _logs.size(); i++)
		{
			CloseLog(i);
			ndDelete(_logs[i]);
		}
		_logs.clear();
	}

	bool LogManager::OpenLog(int num, const std::string& filename, bool addTimestamp)
	{
		if(num >= (int)_logs.size())
		{
			_logs.resize(num+1);
			
			_logs[num] = ndNew0(std::fstream);
			_addTimestamp.resize(num+1, false);
		}

		auto log = std::fstream();
		if( _logs[num]->is_open() )
		{
			CloseLog(num);
		}
		_logs[num]->open(filename, std::ios::out | std::ios::trunc);
		_addTimestamp[num] = addTimestamp;

		return _logs[num]->is_open();
	}

	void LogManager::AddLog(int num, const std::string& txt) _NOEXCEPT
	{
		if(num >= (int)_logs.size() || 
			_logs[num] == 0 ||
			!_logs[num]->is_open())
			return;

		if(_addTimestamp[num])
		{
			static SYSTEMTIME time;
			::GetSystemTime(&time);
			(*_logs[num])<<time.wHour<<':'<<time.wMinute<<':'<<
				time.wSecond<<':'<<time.wMilliseconds<<" - ";
		}
		(*_logs[num])<<txt<<std::endl;
	}

	void LogManager::CloseLog(int num)
	{
		if(_logs[num]->is_open())
			_logs[num]->close();
	}
}