#include "GeneralActions.h"
#include "MainEventPool.h"
#include "Trigger.h"
#include "IntRetrievers.h"
#include "StringRetrievers.h"

namespace FlagRTS
{
	GeneralFamiliyActionFactory::GeneralFamiliyActionFactory()
	{
		_factories["ForInteger"] = xNew0(ForIntegerAction::Factory);
		_factories["Condition"] = xNew0(ConditionAction::Factory);
		_factories["FireTrigger"] = xNew0(FireTriggerAction::Factory);
		_factories["SetVariable"] = xNew0(SetVariableAction::Factory);
	}

	GeneralFamiliyActionFactory::~GeneralFamiliyActionFactory()
	{
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
	}

	IAction* GeneralFamiliyActionFactory::Create(XmlNode* node)
	{
		IAction* action = 0;
		string typeName = XmlUtility::XmlGetString(node, "type");
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			if( facIt->first.compare(typeName) == 0 )
			{
				action = facIt->second->Create(node);
				break;
			}
		}
		return action;
	}

	ForIntegerAction::ForIntegerAction() :
		_actions(0),
		_from(0),
		_increment(0),
		_to(0)
	{ }

	ForIntegerAction::~ForIntegerAction()
	{
		xDeleteSafe(_from);
		xDeleteSafe(_to);
		xDeleteSafe(_increment);
		xDeleteSafe(_actions);
	}

	void ForIntegerAction::Execute()
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_indexVarName);
		for(var = (int)*_from; 
			var < (int)*_to; 
			*var.GetData<int>() += (int)*_increment)
		{
			_actions->Execute();
		}
	}

	void ForIntegerAction::SetParameters(IObjectRetriever<int>* from, 
		IObjectRetriever<int>* to, 
		IObjectRetriever<int>* increment, 
		string index,
		MultiAction* actions)
	{
		_from = from;
		_to = to;
		_increment = increment;
		_indexVarName = index;
		_actions = actions;
	}

	IAction* ForIntegerAction::Factory::Create(XmlNode* actionNode)
	{
		ForIntegerAction* newAction = xNew0(ForIntegerAction);

		XmlNode* fromNode = actionNode->first_node("From");
		XmlNode* toNode = actionNode->first_node("To");
		XmlNode* incNode = actionNode->first_node("IncrementBy");
		XmlNode* varNode = actionNode->first_node("Variable");
		XmlNode* childActionNode = actionNode->first_node("Actions")->first_node();
		MultiAction* actions = xNew0(MultiAction);

		while(childActionNode != 0)
		{
			actions->AddAction(MainEventPool::GlobalPool->
				CreateAction(childActionNode));
			childActionNode = childActionNode->next_sibling();
		}

		newAction->SetParameters(
			IntRetrieverFactory::Create(fromNode),
			IntRetrieverFactory::Create(toNode),
			IntRetrieverFactory::Create(incNode),
			XmlUtility::XmlGetString(varNode, "name"),
			actions);

		return newAction;
	}

	ConditionAction::ConditionAction() :
		_condition(0),
		_action(0)
	{ }

	ConditionAction::~ConditionAction()
	{
		xDeleteSafe(_condition);
		xDeleteSafe(_action);
	}

	void ConditionAction::Execute()
	{
		if( _condition->CheckCondition() )
		{
			_action->Execute();
		}
	}

	void ConditionAction::SetParameters(ICondition* condition, IAction* action)
	{
		_condition = condition;
		_action = action;
	}

	IAction* ConditionAction::Factory::Create(XmlNode* actionNode)
	{
		ConditionAction* newAction = xNew0(ConditionAction);

		XmlNode* conditionNode = actionNode->first_node("Condition");
		XmlNode* childactionNode = actionNode->first_node("Action");

		IAction* action = MainEventPool::GlobalPool->CreateAction(childactionNode);
		ICondition* condition = MainEventPool::GlobalPool->CreateCondition(conditionNode);

		newAction->SetParameters(condition, action);

		return newAction;
	}

	FireTriggerAction::FireTriggerAction()
	{ }

	FireTriggerAction::~FireTriggerAction()
	{ }

	void FireTriggerAction::Execute()
	{
		MainEventPool::GlobalPool->FireTrigger(
			MainEventPool::GlobalPool->FindTrigger(_triggerName));
	}

	void FireTriggerAction::SetParameters(string trigger)
	{
		_triggerName = trigger;
	}

	IAction* FireTriggerAction::Factory::Create(XmlNode* actionNode)
	{
		FireTriggerAction* newAction = xNew0(FireTriggerAction);

		XmlNode* triggerNode = actionNode->first_node("Trigger");
		newAction->SetParameters(
			XmlUtility::XmlGetString(triggerNode, "name"));

		return newAction;
	}

	SetVariableAction::SetVariableAction() :
		_value(0)
	{ }

	SetVariableAction::~SetVariableAction()
	{
		xDeleteSafe(_value);
	}

	void SetVariableAction::Execute()
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_toSet);
		var = _value->operator FlagRTS::Variable();
	}

	void SetVariableAction::SetParameters(string toSet, IObjectRetriever<Variable>* value)
	{
		_toSet = toSet;
		_value = value;
	}

	IAction* SetVariableAction::Factory::Create(XmlNode* actionNode)
	{
		SetVariableAction* newAction = xNew0(SetVariableAction);

		// TODO

		return newAction;
	}
}