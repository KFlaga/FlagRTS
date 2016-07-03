#pragma once

#include "IObjectRetriever.h"
#include "Variable.h"

namespace FlagRTS
{
	class VariableFixedRetriever : public IObjectRetriever<Variable>
	{
		Variable _value;
	public:
		VariableFixedRetriever(const Variable& value) :
			_value(value)
		{ }

		operator Variable() const
		{
			return _value;
		}
		
		IObjectRetriever<Variable>* GetCopy() 
		{ 
			return xNew1(VariableFixedRetriever, _value); 
		}
	};

	class VariableFromNameRetriever : public IObjectRetriever<Variable>
	{
		string _name;
	public:
		VariableFromNameRetriever(const string& name) :
			_name(name)
		{ }

		operator Variable() const;
		
		IObjectRetriever<Variable>* GetCopy() 
		{ 
			return xNew1(VariableFromNameRetriever, _name); 
		}
	};

	class VariableRetrieverFactory
	{
	public:
		static IObjectRetriever<Variable>* Create(XmlNode* valueNode);
	};
}