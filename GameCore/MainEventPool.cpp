#include "MainEventPool.h"
#include <Exception.h>
#include "InputEventPool.h"
#include "SceneObjectActionFamilyFactory.h"
#include "GeneralActions.h"
#include "GeneralConditions.h"
#include "PlayerConditions.h"
#include "GeneralEventPool.h"
#include "InputEventPool.h"


namespace FlagRTS
{
	MainEventPool* MainEventPool::GlobalPool = 0;

	MainEventPool::MainEventPool()
	{
		_eventPools.insert(std::make_pair(
			"General", xNew0(GeneralEventPool)));
		_eventPools.insert(std::make_pair(
			"Input", xNew0(InputEventPool)));

		_actionParsers["General"] = xNew0(GeneralFamiliyActionFactory);
		_actionParsers["SceneObject"] = xNew0(SceneObjectFamilyActionFactory);

		_conditionParsers["General"] = xNew0(GeneralFamilyConditionFactory);
		_conditionParsers["Player"] = xNew0(PlayerFamilyConditionFactory);
	}

	MainEventPool::~MainEventPool()
	{
		RemoveAllTriggers();
		for(auto facIt = _actionParsers.begin(); facIt != _actionParsers.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
		for(auto facIt = _conditionParsers.begin(); facIt != _conditionParsers.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
		for(auto poolIt = _eventPools.begin(); poolIt != _eventPools.end(); ++poolIt)
		{
			xDelete(poolIt->second);
		}
	}

	void MainEventPool::Update(float ms)
	{
		for(auto poolIt = _eventPools.begin(); poolIt != _eventPools.end(); ++poolIt)
		{
			poolIt->second->Update(ms);
		}
	}

	void MainEventPool::ParseAllTriggers(XmlNode* rootNode)
	{
		// trigsNode is root node in map's triggers file
		// First save all variables - find node Variables
		XmlNode* varsNode = rootNode->first_node("Variables", 9);
		// Save all variables 
		// Create triggers - find Triggers node
		XmlNode* trigsNode = rootNode->first_node("Triggers", 8);
		XmlNode* trigNode = trigsNode->first_node("Trigger");
		while(trigNode != 0)
		{
			Trigger* trigger = CreateTrigger(trigNode);
			_triggers.insert(std::make_pair(trigger->GetName(), trigger));

			bool enabled = XmlUtility::XmlGetBoolIfExists(trigNode, "is_enabled", true);
			if(enabled)
				trigger->Enable();

			trigNode = trigNode->next_sibling();
		}
	}

	Trigger* MainEventPool::CreateTrigger(XmlNode* trigNode)
	{
		const char* trigName = XmlUtility::XmlGetString(trigNode, "name");

		Trigger* trigger = xNew1(Trigger, trigName);

		XmlNode* evtNode = trigNode->first_node("Event");
		if(evtNode != 0) // Trigger may not have and event
		{
			trigger->SetEvent(FindEvent(evtNode));
		}

		XmlNode* actsNode = trigNode->first_node("Actions", 7);
		MultiAction* actions = xNew0(MultiAction);
		XmlNode* actNode = actsNode->first_node("Action", 6);
		while (actNode != 0)
		{
			actions->AddAction(CreateAction(actNode));
			actNode = actNode->next_sibling();
		}
		trigger->SetActions(actions);
		return trigger;
	}

	Variable& MainEventPool::CreateVariable(XmlNode* varNode)
	{
		// <Variable name="var_name"/>
		Variable& var = _variables[string(XmlUtility::XmlGetString(varNode, "name"))];
		return var;
	}

	IAction* MainEventPool::CreateAction(XmlNode* actNode)
	{
		//<Action family="A" type="B"/>
		string actFamily = XmlUtility::XmlGetString(actNode, "family", 6);
		auto facIt = _actionParsers.find(actFamily);
		return facIt != _actionParsers.end() ?
			facIt->second->Create(actNode) : 0;
	}

	IEvent* MainEventPool::FindEvent(XmlNode* evtNode)
	{
		//<Event family="General" type="Initialisation"/>
		string evtFamily = XmlUtility::XmlGetString(evtNode, "family", 6);
		auto facIt = _eventPools.find(evtFamily);
		return facIt != _eventPools.end() ?
			facIt->second->FindEvent(XmlUtility::XmlGetString(evtNode, "type", 4)) : 0;
	}

	IEvent* MainEventPool::FindEvent(const string& family, const string& type)
	{
		auto facIt = _eventPools.find(family);
		return facIt != _eventPools.end() ?
			facIt->second->FindEvent(type) : 0;
	}

	ICondition* MainEventPool::CreateCondition(XmlNode* conditionNode)
	{
		//<Condition family="A" type="B"/>
		string condFamily = XmlUtility::XmlGetString(conditionNode, "family", 6);
		auto facIt = _conditionParsers.find(condFamily);
		return facIt != _conditionParsers.end() ?
			facIt->second->Create(conditionNode) : 0;
	}

	void MainEventPool::AddTrigger(Trigger* trigger)
	{
		if(_triggers.count(trigger->GetName()) > 0)
		{
			CastException_d((string("Trigger already registered : ") + trigger->GetName()).c_str());
			return;
		}
		_triggers.insert(std::make_pair(
			trigger->GetName(), trigger));
	}

	void MainEventPool::RemoveTrigger(Trigger* trigger, bool deleteIt)
	{
		auto trigIt = _triggers.find(trigger->GetName());
		if(trigIt == _triggers.end())
		{
			CastException_d((string("Trigger not registered : ") + trigger->GetName()).c_str());
			return;
		}

		_triggers.erase(trigIt);
		if(deleteIt)
			xDelete(trigger);
	}

	void MainEventPool::RemoveTrigger(const string& name, bool deleteIt)
	{
		auto trigIt = _triggers.find(name);
		if(trigIt == _triggers.end())
		{
			CastException_d((string("Trigger not registered : ") + name).c_str());
			return;
		}
		Trigger* trigger = trigIt->second;
		_triggers.erase(trigIt);
		if(deleteIt)
			xDelete(trigger);
	}

	void MainEventPool::RemoveAllTriggers()
	{
		// delete all triggers
		for(auto trigIt = _triggers.begin(); trigIt != _triggers.end(); ++trigIt)
		{
			xDelete(trigIt->second);
		}
		_triggers.clear();
	}

	void MainEventPool::FireTrigger(Trigger* trigger)
	{
		trigger->GetActions()->Execute();
	}

	Trigger* MainEventPool::FindTrigger(const string& name)
	{
		auto trigIt = _triggers.find(name);	
		return trigIt != _triggers.end() ?
			trigIt->second : 0;
	}

	void MainEventPool::MapInitialised()
	{
		static_cast<GeneralEventPool*>(_eventPools["General"])->MapInitialised();
	}
}