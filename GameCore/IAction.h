#pragma once

#include <Event.h>
#include "DataTypes.h"
#include <TypeId.h>
#include <Array.h>
#include "EventArgsRetriever.h"
#include <Xml.h>

namespace FlagRTS
{
	__interface IAction
	{
	public:
		virtual void Execute() = 0;
	};

	template<typename... Args>
	__interface IParametrizedAction : public IAction
	{
	public:
		virtual void SetParameters(Args... args) = 0;
	};

	__interface IActionFactory
	{
	public:
		virtual IAction* Create(XmlNode* actionNode) = 0;
	};

	// Can store and execute multiple IActions
	class MultiAction : public IAction
	{
	protected:
		Array<IAction*> _actions;

	public:
		MultiAction()
		{ }

		void AddAction(IAction* action) { _actions.push_back(action); }
		void RemoveAction(IAction* action) { _actions.remove(action); }

		void Execute()
		{
			for(auto actionIt = _actions.begin(); actionIt != _actions.end(); ++actionIt)
			{
				(*actionIt)->Execute();
			}
		}

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* actionNode);
		};
	};

	template<typename EvtArg1>
	class ActionEventHandler : public EventHandler<EvtArg1>
	{
	protected:
		IAction* _action;

	public:
		ActionEventHandler(IAction* action) :
			_action(action)
		{ }

		void operator()(EvtArg1 arg1)
		{
			EventArgsRetriever::Capture(&arg1);
			_action->Execute();
		}
	};
}