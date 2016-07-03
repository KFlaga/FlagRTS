#pragma once

#include "IAction.h"

namespace FlagRTS
{
	// IEvent is base interface for things that can happen to trigger an action in trigger-system
	// Actions can be added to IEvent, and if it occurs, they are executed
	// If event had some argument ( like key code if it was KeyPressed or Unit if it was UnitDied )
	// it will be stored in Variable named "EventArg_x" where x is parameter index (counting from 0) ( it will store last fired event args )
	// (??? how to determine this index in scripts - possibly same way as its name : from this code for now, 
	// and will need to code such thing into some editor some day )
	// IEvent should be not subclassed directly, ParametrizedEvent should be instead
	__interface IEvent
	{
	public:
		virtual void AddAction(IAction* action) = 0;
		virtual void RemoveAction(IAction* action) = 0;
		virtual unsigned int GetActionsCount() = 0;
	};

	__interface IEventFactory
	{
	public:
		IEvent* Create(XmlNode* evtNode);
	};

	template<typename... Args>
	class ParametrizedEvent : public IEvent
	{
	protected:
		Array<IAction*> _registeredActions;

	public:
		void AddAction(IAction* action)
		{
			_registeredActions.push_back(action);
		}

		void RemoveAction(IAction* action)
		{
			_registeredActions.remove(action);
		}

		unsigned int GetActionsCount()
		{
			return _registeredActions.size();
		}

		virtual void Fire(Args... args) = 0;
	};
}