#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class IntFixedRetriever : public IObjectRetriever<int>
	{
		int _value;
	public:
		IntFixedRetriever(int value) :
			_value(value)
		{ }

		operator int() const
		{
			return _value;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(IntFixedRetriever, _value); 
		}
	};
	
	class IntVariableRetriever : public IObjectRetriever<int>
	{
		string _varName;
	public:
		IntVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator int() const;
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(IntVariableRetriever, _varName); 
		}
	};
	
	class IntRetrieverFactory
	{
	public:
		static IObjectRetriever<int>* Create(XmlNode* valueNode);
	};
}