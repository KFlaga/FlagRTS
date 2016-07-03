#pragma once

#include <map>
#include <Array.h>
#include "DataTypes.h"
#include <Event.h>

namespace FlagRTS
{
	class Unit;
	class CommandTarget;
	class Command;

	class UnitGroup
	{
	public:
		typedef std::map<size_t, Array<Unit*>> UnitMap;

	private:
		UnitMap _units;
		UnitClass _groupClass; // Combined class of all unit types in group
		unsigned int _unitCount;
		Event<UnitGroup*, Unit*> _onGroupUnitDied;

	public:
		UnitGroup();
		~UnitGroup();

		// Adds unit to this group
		void AddToGroup(Unit* unit);

		// Removes unit from this group
		void RemoveFromGroup(Unit* unit);

		// Removes all units from this group
		void RemoveAll()
		{
			_units.clear();
			_unitCount = 0;
		}

		// Returns number of units in this group
		unsigned int GroupCount() const { return _unitCount; }

		// Orders command to all units in group
		void OrderAll(int command, CommandTarget* target, bool queue);

		// Orders command to all units of given type in group
		void OrderAllOfType(size_t type, int command, CommandTarget* target, bool queue);

		// Orders command to all units with given class in group
		void OrderAllWithClass(UnitClass type, int command, CommandTarget* target, bool queue);

		// Returns combined group class
		UnitClass GetGroupClass() const { return _groupClass; }


		UnitMap* GetUnitsMap() { return &_units; }
		// Returns iterator to begining of units map
		UnitMap::iterator GetUnitGroupBegin() { return _units.begin(); }
		// Returns iterator to end of units map
		UnitMap::iterator GetUnitGroupEnd() { return _units.end(); }
		// Returns iterator to position of units with given type 
		UnitMap::iterator GetUnitsOfTypeIterator(size_t type) { return _units.find(type); }
		// Returns list of units with given type
		Array<Unit*>* GetUnitsOfType(size_t type) { return &(_units.find(type)->second); }
		// Removes unit type from group
		void RemoveAllOfType(size_t type)
		{
			_units.erase(type);
		}

		typedef void(*UnitFunc)(Unit*);
		// Takes action for each unit in group
		void ForEach(UnitFunc func)
		{
			for(auto typeIt = _units.begin(), end = _units.end(); typeIt != end; ++typeIt)
			{
				for(auto unitIt = typeIt->second.begin(), end2 = typeIt->second.end(); unitIt != end2; ++unitIt)
				{
					func(*unitIt);
				}
			}
		}

		// Copies units from given group
		void CopyOther(UnitGroup* group); 
		// Add all units from given group to this one
		void AddFromGroup(UnitGroup* group); 
		// Returns unit that represents group ( that is unit with highest priority
		// and with highest count in this priotity )
		Unit* GetGroupRepresentant();

		Event<UnitGroup*, Unit*>& UnitFromGroupDied() { return _onGroupUnitDied; }

	private:
		void UnitDied(Unit* unit);

		DEFINE_DELEGATE1(UnitDiedDelegate, 
			UnitGroup, UnitDied, Unit*);
		UnitDiedDelegate _onUnitDied;
	};
}