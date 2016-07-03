#include "UnitGroup.h"
#include "Unit.h"
#include "UnitClass.h"

namespace FlagRTS
{
	UnitGroup::UnitGroup() :
		_onUnitDied(this)
	{

	}

	UnitGroup::~UnitGroup()
	{

	}

	void UnitGroup::AddToGroup(Unit* unit)
	{
		_ASSERT(unit != 0);
		_units[unit->GetUnitKind()].push_back(unit);
		++_unitCount;
		// Check if group class changes :
		// 1) if unit is first in group set its class
		if(_units.size() == 1)
		{
			_groupClass = unit->GetUnitClass();
		}
		// 2) group class correspond to current units, so mix it with newly added one
		else
		{
			_groupClass |= unit->GetUnitClass();
		}

		unit->UnitDied() += &_onUnitDied;
	}

	void UnitGroup::RemoveFromGroup(Unit* unit)
	{
		_ASSERT(unit != 0);
		auto unitSetIt = _units.find(unit->GetUnitKind());
		if(unitSetIt != _units.end())
		{
			unitSetIt->second.remove(unit);
			if(unitSetIt->second.size() == 0)
				_units.erase(unitSetIt);
			--_unitCount;

			// Check if group class changes
			// here it a bit more complicated, as cannot simply use logical and
			// instead iterate over all unit types and mix its classes
			_groupClass = 0;
			for(unitSetIt = _units.begin(); unitSetIt != _units.end(); ++unitSetIt)
			{
				_groupClass |= (*unitSetIt->second.begin())->GetUnitClass();
			}
			unit->UnitDied() -= &_onUnitDied;
		}
	}

	void UnitGroup::OrderAll(int command, CommandTarget* target, bool queue)
	{
		for(auto typeIt = _units.begin(); typeIt != _units.end(); ++typeIt)
		{
			for(auto unitIt = typeIt->second.begin(); unitIt != typeIt->second.end(); ++unitIt)
			{
				(*unitIt)->ExecuteCommand(command, target, queue);
			}
		}
	}

	void UnitGroup::OrderAllOfType(size_t type, int command, CommandTarget* target, bool queue)
	{
		auto typeIt = _units.find(type);
		for(auto unitIt = typeIt->second.begin(); unitIt != typeIt->second.end(); ++unitIt)
		{
			(*unitIt)->ExecuteCommand(command, target, queue);
		}
	}

	void UnitGroup::UnitDied(Unit* unit)
	{
		// Remove unit from group, but dont remove its on died event handler
		auto unitSetIt = _units.find(unit->GetUnitKind());
		if(unitSetIt != _units.end())
		{
			unitSetIt->second.remove(unit);
			if(unitSetIt->second.size() == 0)
				_units.erase(unitSetIt);
			--_unitCount;

			// Check if group class changes
			// here it a bit more complicated, as cannot simply use logical and
			// instead iterate over all unit types and mix its classes
			_groupClass = 0;
			for(unitSetIt = _units.begin(); unitSetIt != _units.end(); ++unitSetIt)
			{
				_groupClass |= (*unitSetIt->second.begin())->GetUnitClass();
			}
			_onGroupUnitDied.Fire(this, unit);
		}
	}
}