#pragma once

#include "GenericManager.h"
#include <map>
#include "Thread.h"
#include "StringUtils.h"

namespace FlagRTS
{
	// Hold references to all threads and allows to create new ones
	// On releasing if threads aren't referenced in code, they are
	// automaticaly joined
	class ThreadManager : public GenericManager<ThreadManager>
	{
	private:
		typedef std::map<string, Thread*, StringLess> ThreadMap;
		ThreadMap _threads;

	protected:
		ThreadManager();
		friend GenericManager<ThreadManager>;

	public:
		~ThreadManager();

		// Creates new threas with calling thread as parent
		Thread* CreateThread(const string& name);

		// Returns thread with given name or invalid
		// pointer of thread not found
		Thread* GetThread(const string& name);

		// Removes threads that finished work
		void RemoveThread(const string& name);

		// Returns currently working thread, or 0 if its main thread ot thread not found
		Thread* GetCurrentThread();
	};
}