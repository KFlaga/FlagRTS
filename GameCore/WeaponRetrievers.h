#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class Weapon;
	class Unit;

	class WeaponFixedRetriever : public IObjectRetriever<Weapon*>
	{
		Weapon* _value;
	public:
		WeaponFixedRetriever(Weapon* value) :
			_value(value)
		{ }

		operator Weapon*() const
		{
			return _value;
		}
		
		IObjectRetriever<Weapon*>* GetCopy() 
		{ 
			return xNew1(WeaponFixedRetriever, _value); 
		}
	};

	class WeaponNewFromDefinitionRetriever : public IObjectRetriever<Weapon*>
	{
		string _defName;
	public:
		WeaponNewFromDefinitionRetriever(const string& defName) :
			_defName(defName)
		{ }

		operator Weapon*() const;
		
		IObjectRetriever<Weapon*>* GetCopy() 
		{ 
			return xNew1(WeaponNewFromDefinitionRetriever, _defName); 
		}
	};

	class UnitWeaponNameRetriever : public IObjectRetriever<Weapon*>
	{
		IObjectRetriever<Unit*>* _unit;
		string _name;

	public:
		UnitWeaponNameRetriever(const string& name, IObjectRetriever<Unit*>* unit) :
			_name(name),
			_unit(unit)
		{ }
		~UnitWeaponNameRetriever();

		operator Weapon*() const;
		
		IObjectRetriever<Weapon*>* GetCopy() 
		{ 
			return xNew2(UnitWeaponNameRetriever, _name, _unit->GetCopy()); 
		}
	};
	
	class WeaponRetrieverFactory
	{
	public:
		static IObjectRetriever<Weapon*>* Create(XmlNode* valueNode);
	};
}