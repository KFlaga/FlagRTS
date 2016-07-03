#include "Thread.h"
#include "LogManager.h"

namespace FlagRTS
{
	const std::thread::id Thread::MainThreadId = std::this_thread::get_id();

	Thread::Thread(const string& name) : 
		_thread(), 
		_name(name),
		_parent(0),
		_end(false)
	{
		ADD_LOG(string("Creating thread with name = ") + _name);
	}

	Thread::~Thread()
	{
		ADD_LOG(string("Destroying thread with name = ") + _name);
		if(IsRunning())
		{
			if(_parent != 0)
				_parent->RemoveChildThread(this);
			_thread.join(); // if thread is still running, wait for it to finish
		}
	}

	void Thread::End()
	{
		ADD_LOG(string("Ending thread with name = ") + _name);
		// End all child threads
		for(auto it = _childThreads.begin(); it != _childThreads.end(); it++)
		{
			(*it).Value->SetParent(0);
			(*it).Value->End();
		}
		_childThreads.clear();
		_end = true;
	}

	bool Thread::IsRunning() const
	{
		return _thread.joinable();
	}

	void Thread::AddChildThread(Thread* child)
	{
		child->SetParent(this);
		_childThreads.push_back(child);
	}

	void Thread::RemoveChildThread(Thread* child)
	{
		child->SetParent(0);
		_childThreads.remove(child);
	}

	List<Thread*>& Thread::GetChildThreads()
	{
		return _childThreads;
	}

	std::thread::id Thread::GetId() const 
	{ 
		return _thread.get_id(); 
	}

	void Thread::Join()
	{
		if(IsRunning())
		{
			if(_parent != 0)
				_parent->RemoveChildThread(this);
			// Wait for all children to finish
			for(auto it = _childThreads.begin(); it != _childThreads.end(); it++)
			{
				(*it).Value->SetParent(0);
				(*it).Value->Join();
			}
			_childThreads.clear();

			_thread.join();
		}
	}
}