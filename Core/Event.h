#pragma once

#include "List.h"

namespace FlagRTS
{
	// Class to be overriden to call other classes methods
	// with given args
	template<typename... Args>
	class EventHandler
	{
	public:
		EventHandler()  { }

		virtual void operator()(Args... args) = 0;
	};

	// Event handler that executes given function
	template<typename FncType, typename... Args>
	class DelegateEventHandler : public EventHandler<Args...>
	{
	private:
		FncType _callback;

	public:
		DelegateEventHandler(FncType fnc) : _callback(fnc) { }

		virtual void operator()(Args... args)
		{
			_callback(args...);
		}
	};

	template<typename... Args>
	class Event
	{
	private:
		List<EventHandler<Args...>*> _handlers;

	public:
		Event() { }

		const List<EventHandler<Args...>*>& GetHandlers() const { return _handlers; }

		void AddHandler( EventHandler<Args...>* handler )
		{
			_handlers.push_back(handler);
		}

		void RemoveHandler( EventHandler<Args...>* handler )
		{
			_handlers.remove(handler);
		}

		void Clear()
		{
			_handlers.clear();
		}

		void DeleteAll()
		{
			for(auto it = _handlers.begin(); it != _handlers.end(); ++it)
			{
				xDelete(it->Value);
			}
			Clear();
		}

		void Fire(Args... args)
		{
			for(auto it = _handlers.begin(); it != _handlers.end(); ++it)
			{
				(**it)(args...);
			}
		}

		void operator()(Args... args)
		{
			Fire(args...);
		}

		void operator+=( EventHandler<Args...>* handler )
		{
			AddHandler(handler);
		}

		void operator-=( EventHandler<Args...>* handler )
		{
			RemoveHandler(handler);
		}

		void operator+=( const Event& evt )
		{
			for(auto it = evt._handlers.cbegin(); it != evt._handlers.cend(); ++it)
			{
				this->AddHandler(*it);
			}
		}

		void operator-=( const Event& evt )
		{
			for(auto it = evt._handlers.cbegin(); it != evt._handlers.cend(); ++it)
			{
				this->RemoveHandler(*it);
			}
		}
	};

#define DEFINE_DELEGATE0(EVName, ClassName, FunName) \
	class EVName : public EventHandler<> \
	{ \
	private: \
		ClassName* _owner; \
	public: \
		EVName(ClassName* owner) : _owner(owner) { } \
		void operator()() { _owner->FunName(); } \
	}; \
	friend EVName;

#define DEFINE_DELEGATE1(EVName, ClassName, FunName, Arg1) \
	class EVName : public EventHandler<Arg1> \
	{ \
	private: \
		ClassName* _owner; \
	public: \
		EVName(ClassName* owner) : _owner(owner) { } \
		void operator()(Arg1 arg1) { _owner->FunName(arg1); } \
	}; \
	friend EVName;

#define DEFINE_DELEGATE2(EVName, ClassName, FunName, Arg1, Arg2) \
	class EVName : public EventHandler<Arg1, Arg2> \
	{ \
	private: \
		ClassName* _owner; \
	public: \
		EVName(ClassName* owner) : _owner(owner) { } \
		void operator()(Arg1 arg1, Arg2 arg2) { _owner->FunName(arg1,arg2); } \
	}; \
	friend EVName;

#define DEFINE_DELEGATE3(EVName, ClassName, FunName, Arg1, Arg2, Arg3) \
	class EVName : public EventHandler<Arg1, Arg2, Arg3> \
	{ \
	private: \
		ClassName* _owner; \
	public: \
		EVName(ClassName* owner) : _owner(owner) { } \
		void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) { _owner->FunName(arg1,arg2,arg3); } \
	}; \
	friend EVName;
}