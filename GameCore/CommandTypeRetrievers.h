#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class Unit;

	class CommandTypeFixedRetriever : public IObjectRetriever<int>
	{
		int _value;
	public:
		CommandTypeFixedRetriever(int value) :
			_value(value)
		{ }

		operator int() const
		{
			return _value;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(CommandTypeFixedRetriever, _value); 
		}
	};

	// Gets command with given name from unit ( used for abilities / other customs )
	class UnitCommandNameRetriever : public IObjectRetriever<int>
	{
		string _commName;
		IObjectRetriever<Unit*>* _unit;
	public:
		UnitCommandNameRetriever(const string& commandName, IObjectRetriever<Unit*>* unit) :
			_commName(_commName),
			_unit(unit)
		{ }
		~UnitCommandNameRetriever();

		operator int() const;
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew2(UnitCommandNameRetriever, _commName, _unit->GetCopy()); 
		}
	};
	
	class CommandTypeRetrieverFactory
	{
	public:
		static IObjectRetriever<int>* Create(XmlNode* valueNode);
	};
}