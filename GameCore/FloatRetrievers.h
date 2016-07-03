#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class FloatFixedRetriever : public IObjectRetriever<float>
	{
		float _value;
	public:
		FloatFixedRetriever(float value) :
			_value(value)
		{ }

		operator float() const
		{
			return _value;
		}
		
		IObjectRetriever<float>* GetCopy() 
		{ 
			return xNew1(FloatFixedRetriever, _value); 
		}
	};
	

	class FloatVariableRetriever : public IObjectRetriever<float>
	{
		string _varName;
	public:
		FloatVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator float() const;
		
		IObjectRetriever<float>* GetCopy() 
		{ 
			return xNew1(FloatVariableRetriever, _varName); 
		}
	};
	
	class FloatRetrieverFactory
	{
	public:
		static IObjectRetriever<float>* Create(XmlNode* valueNode);
	};
}