#pragma once

#include "List.h"
#include "TypeDefs.h"
#include <thread>
#include "Pointers.h"

namespace FlagRTS
{
	// Wrapper around std::thread
	// Contains list of child threads and joins all
	// child threads when destroyed
	// 'Run' with function and arguments must be called to begin execution
	class Thread
	{
	private:
		std::thread _thread;
		Thread* _parent;
		List<Thread*> _childThreads;
		string _name;
		bool _end;

	public:
		static const std::thread::id MainThreadId;
		static bool IsMainThread(const std::thread::id& id) { return id == MainThreadId; }
		static bool IsMainThread() { return std::this_thread::get_id() == MainThreadId; }

		Thread(const string& name);
		~Thread();

		// Returns true of thread os executing ( is joinable )
		bool IsRunning() const;

		// Starts thread execution
		template <typename Fn, typename... Args>
		void Run(Fn function, Args... args)
		{
			_thread = std::thread(function, args...);
		}

		// To be called when should end
		void End();

		bool ShouldEnd() const { return _end; }

		// Adds thread to list of children - for thread-join hierarhy
		void AddChildThread(Thread* child);
		// Removes thread from list of children, which means child is joined
		void RemoveChildThread(Thread* child);

		// Returns list of child threads
		List<Thread*>& GetChildThreads();

		Thread* GetParent() const { return _parent; }
		void SetParent(Thread* thread) { _parent = thread; }

		// Returns unique thread id (unless not joinable)
		std::thread::id GetId() const;

		// Joins thread if joinable
		void Join();

		size_t GetHandle()
		{
			return reinterpret_cast<size_t>(this);
		}

		static Thread* GetFromHandle(const size_t handle)
		{
			return reinterpret_cast<Thread*>(handle);
		}
	};
}