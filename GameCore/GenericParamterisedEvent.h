#pragma once

#include "IEvent.h"
#include "MainEventPool.h"

namespace FlagRTS
{
	// Some generic templates/defines to make defining IEvents faster
	class ParametrizedEvent_0 : public ParametrizedEvent<>
	{
	public:
		void Fire()
		{
			// As event doesnt have parameters, do not set varaible
			// Execute all actions
			for(unsigned int i = 0; i < _registeredActions.size(); ++i)
			{
				_registeredActions[i]->Execute();
			}
		}
	};

	template<typename Arg1T>
	class ParametrizedEvent_1 : public ParametrizedEvent<Arg1T>
	{
	public:
		void Fire(Arg1T arg1)
		{
			// Set variable
			Variable& var = MainEventPool::GlobalPool->FindVariable("EventArg_0");
			var = arg1;
			// Execute all actions
			for(unsigned int i = 0; i < _registeredActions.size(); ++i)
			{
				_registeredActions[i]->Execute();
			}
		}
	};

	template<typename Arg1T, typename Arg2T>
	class ParametrizedEvent_2 : public ParametrizedEvent<Arg1T, Arg2T>
	{
	public:
		void Fire(Arg1T arg1, Arg2T arg2)
		{
			// Set variable
			Variable& var1 = MainEventPool::GlobalPool->FindVariable("EventArg_0");
			var1 = arg1;
			Variable& var2 = MainEventPool::GlobalPool->FindVariable("EventArg_1");
			var2 = arg2;

			// Execute all actions
			for(unsigned int i = 0; i < _registeredActions.size(); ++i)
			{
				_registeredActions[i]->Execute();
			}
		}
	};

	template<typename Arg1T, typename Arg2T, typename Arg3T>
	class ParametrizedEvent_3 : public ParametrizedEvent<Arg1T, Arg2T, Arg3T>
	{
	public:
		void Fire(Arg1T arg1, Arg2T arg2, Arg3T arg3)
		{
			// Set variable
			Variable& var1 = MainEventPool::GlobalPool->FindVariable("EventArg_0");
			var1 = arg1;
			Variable& var2 = MainEventPool::GlobalPool->FindVariable("EventArg_1");
			var2 = arg2;
			Variable& var3 = MainEventPool::GlobalPool->FindVariable("EventArg_2");
			var3 = arg3;

			// Execute all actions
			for(unsigned int i = 0; i < _registeredActions.size(); ++i)
			{
				_registeredActions[i]->Execute();
			}
		}
	};

	// Most events dont fave any parameters in Xml so here's fast factory define
#define DEFINE_SIMPLE_EVENTFACTORY(EventT) \
	struct Factory : public IEventFactory { \
		IEvent* Create(XmlNode* evtNode) { \
		return xNew0(EventT); } };
}