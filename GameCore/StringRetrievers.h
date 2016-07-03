#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class StringFixedRetriever : public IObjectRetriever<const string&>
	{
		string _value;
	public:
		StringFixedRetriever(const string& value) :
			_value(value)
		{ }

		operator const string&() const
		{
			return _value;
		}

		operator string() const
		{
			return _value;
		}
		
		IObjectRetriever<const string&>* GetCopy() 
		{ 
			return xNew1(StringFixedRetriever, _value); 
		}
	};
	
	class StringXmlRetriever : public IObjectRetriever<const string&>
	{
		string _value;
	public:
		StringXmlRetriever(XmlNode* valueNode)
		{ 
			_value = XmlUtility::XmlGetString(valueNode, "value");
		}
		
		operator const string&() const
		{
			return _value;
		}
		
		IObjectRetriever<const string&>* GetCopy() 
		{ 
			return xNew1(StringFixedRetriever, _value); 
		}
	};
	
	class StringRetrieverFactory
	{
	public:
		static IObjectRetriever<const string&>* Create(XmlNode* valueNode);
	};
}