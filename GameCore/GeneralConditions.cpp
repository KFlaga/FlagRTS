#include "GeneralConditions.h"
#include "MainEventPool.h"
#include "VariableRetrievers.h"

namespace FlagRTS
{
	GeneralFamilyConditionFactory::GeneralFamilyConditionFactory()
	{
		_factories["And"] = xNew0(AndCondition::Factory);
		_factories["Or"] = xNew0(OrCondition::Factory);
		_factories["Not"] = xNew0(NotCondition::Factory);
		_factories["Equals"] = xNew0(EqualsCondition::Factory);
		_factories["NotEquals"] = xNew0(NotEqualsCondition::Factory);
		_factories["Greater"] = xNew0(GreaterCondition::Factory);
		_factories["Lesser"] = xNew0(LesserCondition::Factory);
	}

	GeneralFamilyConditionFactory::~GeneralFamilyConditionFactory()
	{
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
	}

	ICondition* GeneralFamilyConditionFactory::Create(XmlNode* condNode)
	{
		ICondition* condition = 0;
		string typeName = XmlUtility::XmlGetString(condNode, "type");
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			if( facIt->first.compare(typeName) == 0 )
			{
				condition = facIt->second->Create(condNode);
			}
		}
		return condition;
	}

	AndCondition::~AndCondition()
	{
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			xDelete(_conditions[i]);
		}
	}

	bool AndCondition::CheckCondition()
	{
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			if( _conditions[i]->CheckCondition() == false )
				return false;
		}
		return true;
	}

	AndCondition* AndCondition::GetCopy()
	{
		AndCondition* cond = xNew0(AndCondition);
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			cond->_conditions.push_back(_conditions[i]->GetCopy());
		}
		return cond;
	}

	ICondition* AndCondition::Factory::Create(XmlNode* condNode)
	{
		AndCondition* newCond = xNew0(AndCondition);
		// Get first node -> it should be Condition
		XmlNode* childCondNode = condNode->first_node("Condition");
		while(childCondNode != 0)
		{
			newCond->AddCondition( MainEventPool::GlobalPool->CreateCondition(childCondNode) );
			childCondNode = childCondNode->next_sibling("Condition");
		}
		return newCond;
	}

	bool OrCondition::CheckCondition()
	{
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			if( _conditions[i]->CheckCondition() == true )
				return true;
		}
		return false;
	}

	OrCondition::~OrCondition()
	{
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			xDelete(_conditions[i]);
		}
	}

	OrCondition* OrCondition::GetCopy()
	{
		OrCondition* cond = xNew0(OrCondition);
		for(unsigned int i = 0; i < _conditions.size(); ++i)
		{
			cond->_conditions.push_back(_conditions[i]->GetCopy());
		}
		return cond;
	}

	ICondition* OrCondition::Factory::Create(XmlNode* condNode)
	{
		OrCondition* newCond = xNew0(OrCondition);
		// Get first node -> it should be Condition
		XmlNode* childCondNode = condNode->first_node("Condition");
		while(childCondNode != 0)
		{
			newCond->AddCondition( MainEventPool::GlobalPool->CreateCondition(childCondNode) );
			childCondNode = childCondNode->next_sibling("Condition");
		}
		return newCond;
	}

	NotCondition::~NotCondition()
	{
		xDelete(_condition);
	}

	bool NotCondition::CheckCondition()
	{
		return _condition != 0 ? !_condition->CheckCondition() : false;
	}

	NotCondition* NotCondition::GetCopy()
	{
		NotCondition* cond = xNew0(NotCondition);
		cond->_condition = _condition->GetCopy();
		return cond;
	}

	ICondition* NotCondition::Factory::Create(XmlNode* condNode)
	{
		NotCondition* newCond = xNew0(NotCondition);
		// Get first node -> it should be Condition
		XmlNode* childCondNode = condNode->first_node("Condition");
		if(childCondNode != 0)
			newCond->SetCondition( MainEventPool::GlobalPool->CreateCondition(childCondNode) );
		childCondNode = childCondNode->next_sibling("Condition");
		return newCond;
	}

	EqualsCondition::~EqualsCondition()
	{
		xDelete(_var1);
		xDelete(_var2);
	}

	bool EqualsCondition::CheckCondition()
	{
		return _var1->operator FlagRTS::Variable() == _var2->operator FlagRTS::Variable();
	}

	EqualsCondition* EqualsCondition::GetCopy()
	{
		EqualsCondition* cond = xNew0(EqualsCondition);
		cond->_var1 = _var1->GetCopy();
		cond->_var2 = _var2->GetCopy();
		return cond;
	}

	ICondition* EqualsCondition::Factory::Create(XmlNode* condNode)
	{
		// Node should have 'var1', 'var2' attributes with variable names
		string var1Name = XmlUtility::XmlGetString(condNode, "var1");
		string var2Name = XmlUtility::XmlGetString(condNode, "var2");

		EqualsCondition* cond = xNew0(EqualsCondition);

		cond->SetParameters(xNew1(VariableFromNameRetriever,var1Name),
			xNew1(VariableFromNameRetriever,var2Name));

		return cond;
	}

	NotEqualsCondition::~NotEqualsCondition()
	{
		xDelete(_var1);
		xDelete(_var2);
	}

	bool NotEqualsCondition::CheckCondition()
	{
		return _var1->operator FlagRTS::Variable() != _var2->operator FlagRTS::Variable();
	}

	NotEqualsCondition* NotEqualsCondition::GetCopy()
	{
		NotEqualsCondition* cond = xNew0(NotEqualsCondition);
		cond->_var1 = _var1->GetCopy();
		cond->_var2 = _var2->GetCopy();
		return cond;
	}

	ICondition* NotEqualsCondition::Factory::Create(XmlNode* condNode)
	{
		// Node should have 'var1', 'var2' attributes with variable names
		string var1Name = XmlUtility::XmlGetString(condNode, "var1");
		string var2Name = XmlUtility::XmlGetString(condNode, "var2");

		NotEqualsCondition* cond = xNew0(NotEqualsCondition);

		cond->SetParameters(xNew1(VariableFromNameRetriever,var1Name),
			xNew1(VariableFromNameRetriever,var2Name));
		return cond;
	}

	GreaterCondition::~GreaterCondition()
	{
		xDelete(_var1);
		xDelete(_var2);
	}

	bool GreaterCondition::CheckCondition()
	{
		return _var1->operator FlagRTS::Variable() > _var2->operator FlagRTS::Variable();
	}

	GreaterCondition* GreaterCondition::GetCopy()
	{
		GreaterCondition* cond = xNew0(GreaterCondition);
		cond->_var1 = _var1->GetCopy();
		cond->_var2 = _var2->GetCopy();
		return cond;
	}

	ICondition* GreaterCondition::Factory::Create(XmlNode* condNode)
	{
		// Node should have 'var1', 'var2' attributes with variable names
		string var1Name = XmlUtility::XmlGetString(condNode, "var1");
		string var2Name = XmlUtility::XmlGetString(condNode, "var2");

		GreaterCondition* cond = xNew0(GreaterCondition);

		cond->SetParameters(xNew1(VariableFromNameRetriever,var1Name),
			xNew1(VariableFromNameRetriever,var2Name));
		return cond;
	}

	LesserCondition::~LesserCondition()
	{
		xDelete(_var1);
		xDelete(_var2);
	}

	bool LesserCondition::CheckCondition()
	{
		return _var1->operator FlagRTS::Variable() < _var2->operator FlagRTS::Variable();
	}

	LesserCondition* LesserCondition::GetCopy()
	{
		LesserCondition* cond = xNew0(LesserCondition);
		cond->_var1 = _var1->GetCopy();
		cond->_var2 = _var2->GetCopy();
		return cond;
	}

	ICondition* LesserCondition::Factory::Create(XmlNode* condNode)
	{
		// Node should have 'var1', 'var2' attributes with variable names
		string var1Name = XmlUtility::XmlGetString(condNode, "var1");
		string var2Name = XmlUtility::XmlGetString(condNode, "var2");

		LesserCondition* cond = xNew0(LesserCondition);

		cond->SetParameters(xNew1(VariableFromNameRetriever,var1Name),
			xNew1(VariableFromNameRetriever,var2Name));
		return cond;
	}
}