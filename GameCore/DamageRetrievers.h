#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class DamageFixedRetriever : public IObjectRetriever<int>
	{
		int _value;
	public:
		DamageFixedRetriever(int value) :
			_value(value)
		{ }

		operator int() const
		{
			return _value;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(DamageFixedRetriever, _value); 
		}
	};
	
	class DamageXmlRetriever : public IObjectRetriever<int>
	{
		int _value;
	public:
		DamageXmlRetriever(XmlNode* valueNode)
		{ 
			
		}
		
		operator int() const
		{
			return _value;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(DamageFixedRetriever, _value); 
		}
	};
	
	class DamageRetrieverFactory
	{
	public:
		static IObjectRetriever<int>* Create(XmlNode* valueNode);
	};
}