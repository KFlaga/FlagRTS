#include "Allocator.h"
#include "LogManager.h"
#include "MemoryPool.h"
#include "Exception.h"

namespace FlagRTS
{
	Allocator* Malloc = 0;

	Allocator::Allocator()
	{

	}

	Allocator::~Allocator()
	{

	}

	MemoryChunk* Allocator::AllocInternal_d(const size_t size, const unsigned int count, const char* file, int line, const char* type)
	{
		if( LogManager::Instance() != 0 )
		{
			if( count == 1 )
			{
			LogManager::Instance()->AddLog(LogManager::MemoryLog, string("Allocating ") +
				std::to_string(size) + " bytes for : " + type +
				" . File: " + file + ", line: " + std::to_string(line));
			}
			else if( count > 1 )
			{
				LogManager::Instance()->AddLog(LogManager::MemoryLog, string("Allocating ") +
				std::to_string(size) + "x" + std::to_string(count) + " bytes for : " + type +
				" . File: " + file + ", line: " + std::to_string(line));
			}
			else
			{
				_ASSERT(false);
			}
		}
		try
		{
			return AllocInternal(size, count);
		}
		catch(std::exception e)
		{
			Exception* exc = new Exception((string("Bad allocation of type : ") + type + ". StdExc: " + e.what()).c_str(), file, line);
			exc->ShowMessageBox();
			throw exc;
		}
		catch(Exception* exc)
		{
			exc->ShowMessageBox();
			throw exc;
		}
	}

	void Allocator::DeallocInternal_d(MemoryChunk* ptr, const char* file, int line, const char* type)
	{
		if( LogManager::Instance() != 0 )
		{
			LogManager::Instance()->AddLog(LogManager::MemoryLog, string("Deleting object : ") +
				type + " . File: " + file + ", line: " + std::to_string(line));
		}
		try
		{
			DeallocInternal(ptr);
		}
		catch(std::exception e)
		{
			Exception* exc = new Exception((string("Bad deallocation of type : ") + type + ". StdExc: " + e.what()).c_str(), file, line);
			exc->ShowMessageBox();
			throw exc;
		}
		catch(Exception* exc)
		{
			exc->ShowMessageBox();
			throw exc;
		}
	}
}
