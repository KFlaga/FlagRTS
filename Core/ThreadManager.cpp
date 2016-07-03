#include "ThreadManager.h"
#include "LogManager.h"
#include "Exception.h"

namespace FlagRTS
{
	ThreadManager* ThreadManager::_instance = 0;

	ThreadManager::ThreadManager() 
	{

	}
	
	ThreadManager::~ThreadManager()
	{
		for(auto it = _threads.begin(); it != _threads.end(); ++it)
		{
			xDelete(it->second);
		}
		_threads.clear();
	}

	Thread* ThreadManager::CreateThread(const string& name)
	{
		Thread* thread = xNew1(Thread,name);
		_threads.insert(std::make_pair(name, thread));
		return thread;
	}

	Thread* ThreadManager::GetThread(const string& name)
	{
		auto threadIt = _threads.find(name);
		if(threadIt != _threads.end())
		{
			return threadIt->second;
		}
		else
		{
			ADD_LOG(string("ThreadManager: Could not find thread with name = ") + name);
			return 0;
		}
	}

	void ThreadManager::RemoveThread(const string& name)
	{
		auto threadIt = _threads.find(name);
		if(threadIt != _threads.end())
		{
			threadIt->second->End();
			xDelete(threadIt->second);
			_threads.erase(threadIt);
		}
		else
		{
			ADD_LOG(string("ThreadManager: Could not find thread with name = ") + name);
		}
	}

	Thread* ThreadManager::GetCurrentThread()
	{
		if(Thread::IsMainThread())
			return 0;

		auto id = std::this_thread::get_id();
		for(auto it = _threads.begin(); it != _threads.end(); ++it)
		{
			if(it->second->GetId() == id)
			{
				return it->second;
			}
		}

		CastException("Working on non-registered, non-main thread");
	}
}