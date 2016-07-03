#pragma once

#include "IAction.h"
#include "IObjectRetriever.h"
#include "Variable.h"
#include <Xml.h>
#include "ICondition.h"

namespace FlagRTS
{
	class GeneralFamiliyActionFactory : public IActionFactory
	{
		std::map<string, IActionFactory*> _factories;
	public:
		GeneralFamiliyActionFactory();
		~GeneralFamiliyActionFactory();

		IAction* Create(XmlNode* node);
	};

	class ForIntegerAction : public IParametrizedAction<
		IObjectRetriever<int>*,
		IObjectRetriever<int>*,
		IObjectRetriever<int>*,
		string,
		MultiAction*>
	{
		IObjectRetriever<int>* _from;
		IObjectRetriever<int>* _to;
		IObjectRetriever<int>* _increment;
		string _indexVarName;
		MultiAction* _actions;

	public:
		ForIntegerAction();
		~ForIntegerAction();

		void Execute();

		void SetParameters(IObjectRetriever<int>* from, 
			IObjectRetriever<int>* to, 
			IObjectRetriever<int>* increment, 
			string indexVarName,
			MultiAction* actions);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* actionNode);
		};
	};

	class ConditionAction : public IParametrizedAction<
		ICondition*,
		IAction*>
	{
		ICondition* _condition;
		IAction* _action;

	public:
		ConditionAction();
		~ConditionAction();

		void Execute();

		void SetParameters(ICondition* condition, IAction* action);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* actionNode);
		};
	};

	class FireTriggerAction : public IParametrizedAction<
		string>
	{
		string _triggerName;

	public:
		FireTriggerAction();
		~FireTriggerAction();

		void Execute();

		void SetParameters(string trigger);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* actionNode);
		};
	};

	class SetVariableAction : public IParametrizedAction<
		string,
		IObjectRetriever<Variable>*>
	{
		string _toSet;
		IObjectRetriever<Variable>* _value;

	public:
		SetVariableAction();
		~SetVariableAction();

		void Execute();

		void SetParameters(string toSet, IObjectRetriever<Variable>* value);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* actionNode);
		};
	};
}