#pragma once

#include "TypeDefs.h"
#include "GenericManager.h"
#include <fstream>
#include "Array.h"

namespace FlagRTS
{
	class LogManager : public GenericManager<LogManager>
	{
	private:
		Array<std::fstream*> _logs;
		Array<bool> _addTimestamp;
		
	protected:
		LogManager();
		friend GenericManager<LogManager>;

	public:
		// Opens log with given path with given id
		// if addTimeStamp = true, then each entry is preceeded with timestamp
		bool OpenLog(int num, const std::string& file, bool addTimeStamp);
		// Adds log entry to given log
		void AddLog(int num, const std::string& txt) _NOEXCEPT;
		// Closes given log
		void CloseLog(int num);

		~LogManager();

		enum Logs
		{
			GeneralLog = 0,
			MemoryLog = 1,
			ErrorLog = 2
		};
	};
}

#define ADD_LOG(txt) LogManager::Instance()->AddLog(LogManager::GeneralLog, (txt))
#define ADD_MEMLOG(txt) LogManager::Instance()->AddLog(LogManager::MemoryLog, (txt))
#define ADD_ERRLOG(txt) LogManager::Instance()->AddLog(LogManager::ErrorLog, (txt))