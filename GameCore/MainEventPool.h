#pragma once

#include "IEventPool.h"
#include "IEvent.h"
#include "IAction.h"
#include "Trigger.h"
#include "Variable.h"
#include "ICondition.h"

#include <map>

#undef CreateEvent

namespace FlagRTS
{
	// Parses scripts ( trigger ) files ( xmls ) and registers all
	// parsed triggers. Afterwards stores them and owns the pointers;
	// On destruction unregisters and deletes all triggers
	// Holds all event pool and event/action/condition factories
	// 
	class MainEventPool
	{
	public:
		static MainEventPool* GlobalPool;

	private:
		// All registered triggers mapped by name
		std::map<string, Trigger*> _triggers;
		
		// All action factories mapped by its family name
		std::map<string, IActionFactory*> _actionParsers;
		
		// All condition factories mapped by its family name
		std::map<string, IConditionFactory*> _conditionParsers;

		// All event pools mapped by its family type
		std::map<string, IEventPool*> _eventPools;

		// All varaibles defined in script file
		std::map<string, Variable> _variables;
	public:
		MainEventPool();
		~MainEventPool();

		// Parses and registers all triggers from given trigger list node
		void ParseAllTriggers(XmlNode* evtsNode);

		// Adds trigger to pool. 
		// After this call MainEventPool owns the pointer
		// It does not enable it
		void AddTrigger(Trigger* trigger);

		// Removes trigger from pool.
		// If 'deleteTrigger' is true, it will be deleted
		void RemoveTrigger(Trigger* trigger, bool deleteTrigger = true);
		void RemoveTrigger(const string& name, bool deleteTrigger = true);

		// Unregister all triggers from trigger map and deletes them
		void RemoveAllTriggers();

		// Processes events from all event pools
		void Update(float ms);

		// Should be called after world is created and before first update
		// It calls Initialisation event
		void MapInitialised();

		// Adds event pool to pool map. After this MainEventPool 
		// have ownership of pool pointer
		void AddEventPool(IEventPool* pool)
		{
			_eventPools.insert(std::make_pair(
				pool->GetEventFamilyName(), pool));
		}
		
		// Adds action factory. After this MainEventPool 
		// have ownership of factory pointer
		void AddActionFactory(IActionFactory* factory, const string& family)
		{
			_actionParsers.insert(std::make_pair(family, factory));
		}

		// Adds condition factory. After this MainEventPool 
		// have ownership of factory pointer
		void AddActionFactory(IConditionFactory* factory, const string& family)
		{
			_conditionParsers.insert(std::make_pair(family, factory));
		}

		IAction* CreateAction(XmlNode* actionNode);
		
		ICondition* CreateCondition(XmlNode* conditionNode);
		
		IEvent* FindEvent(XmlNode* evtNode);

		IEvent* FindEvent(const string& familyName, const string& typeName);

		Variable& CreateVariable(XmlNode* varNode);

		Variable& CreateVariable(const string& name)
		{
			_variables[name] = Variable();
			return _variables[name];
		}

		template<typename ValT>
		Variable& CreateVariable(const string& name, ValT val)
		{
			_variables[name] = val;
			return _variables[name];
		}

		Variable& FindVariable(const string& name)
		{
			return _variables[name];
		}

		Trigger* CreateTrigger(XmlNode* trigNode);

		// Execute actions from trigger
		void FireTrigger(Trigger* trigger);

		Trigger* FindTrigger(const string& name);
	};
}