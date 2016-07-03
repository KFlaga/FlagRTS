#include "PlayerStatsManager.h"
#include "Unit.h"
#include "UnitClass.h"

namespace FlagRTS
{
	PlayerStatsManager::PlayerStatsManager(uint8 player) :
		_player(player),
		_onConstructionFinished(this)
	{

	}

	PlayerStatsManager::~PlayerStatsManager()
	{

	}

	void PlayerStatsManager::UnitSpawned(Unit* unit)
	{
		// First check if unit-kind info exists
		auto unitKindIt = _ownedUnits.find(unit->GetUnitKind());
		if( unitKindIt == _ownedUnits.end() )
		{
			_ownedUnits.insert( unit->GetUnitKind(), xNew0(UnitKindInfo) );
			unitKindIt = _ownedUnits.find(unit->GetUnitKind());
		}
		// Add unit to unit list
		unitKindIt->Value->Units.insert( unit->GetHandle().Object, unit);
		_unitSpawned.Fire(unit, _player);
		// If unit is not constructed -> add event handler to increment object count, otherwise
		// do it immediately and add score for construction
		if( unit->IsUnderConstruction() )
		{
			unit->ConstructionFinished() += &_onConstructionFinished;
		}
		else
		{
			++(unitKindIt->Value->ConstructedCount);
			if( UnitClasses::IsUnitOfSubClass(unit->GetUnitClass(), UnitClasses::Structure) )
			{
				AddConstructedStructureScore(unit);
			}
			else
			{
				AddConstructedUnitScore(unit);
			}
			_unitConstructed.Fire(unit, _player);
		}
	}

	void PlayerStatsManager::UnitKilled(Unit* unit)
	{
		// TODO

		_unitKilled.Fire(unit, _player);
	}

	void PlayerStatsManager::UnitVanished(Unit* unit)
	{
		// TODO

		_unitKilled.Fire(unit, _player);
	}

	void PlayerStatsManager::UnitConstructionFinished(Unit* builder, Unit* unit, bool wasAborted)
	{
		if( wasAborted == false )
		{
			// Find kind info and increment its count
			_ownedUnits[unit->GetUnitKind()]->ConstructedCount += 1;
			// Add score for construction
			if( UnitClasses::IsUnitOfSubClass(unit->GetUnitClass(), UnitClasses::Structure) )
			{
				AddConstructedStructureScore(unit);
			}
			else
			{
				AddConstructedUnitScore(unit);
			}
			_unitConstructed.Fire(unit, _player);
		}
	}

	inline void PlayerStatsManager::AddConstructedUnitScore(Unit* unit)
	{
		++_score.BuildUnitsCount;
	}

	inline void PlayerStatsManager::AddConstructedStructureScore(Unit* unit)
	{
		++_score.BuildStructuresCount;
	}

	inline void PlayerStatsManager::AddKilledUnitScore(Unit* unit)
	{
		++_score.KilledUnitsCount;
	}

	inline void PlayerStatsManager::AddKilledStructureScore(Unit* unit)
	{
		++_score.KilledStructuresCount;
	}

	void PlayerStatsManager::AddLostUnitScore(Unit* unit)
	{
		++_score.LostUnitsCount;
	}

	void PlayerStatsManager::AddLostStructureScore(Unit* unit)
	{
		++_score.LostStructuresCount;
	}

	Unit* PlayerStatsManager::FindClosestUnitOfKind(const Vector3& point, size_t kind)
	{
		auto units = GetAllUnitsOfKind(kind);
		if(units != 0)
		{
			float minDist = 1e24f;
			float dist = 0.f;
			Unit* closest = 0;
			for(auto unitIt = units->begin(); unitIt != units->end(); ++unitIt)
			{
				dist = unitIt->Value->GetPositionAbsolute().squaredDistance(point);
				if( dist < minDist )
				{
					closest = unitIt->Value;
					minDist = dist;
				}
			}
			return closest;
		}
		else
			return 0;
	}

	GlobalStatisticsManager::GlobalStatisticsManager(uint8 playersCount)
	{
		_playersStats.resize(playersCount);
		for(unsigned int i = 0; i < playersCount; ++i)
		{
			_playersStats[i] = xNew1(PlayerStatsManager, i);
		}
	}

	GlobalStatisticsManager::~GlobalStatisticsManager()
	{
		for(unsigned int i = 0; i < _playersStats.size(); ++i)
		{
			xDelete(_playersStats[i]);
		}
	}

	void GlobalStatisticsManager::UnitSpawned(Unit* unit)
	{
		uint8 owner = unit->GetOwner();
		_playersStats[owner]->UnitSpawned(unit);
	}

	void GlobalStatisticsManager::UnitKilled(Unit* unit)
	{
		uint8 owner = unit->GetOwner();
		_playersStats[owner]->UnitKilled(unit);
		// Find killer and add score
	}

	void GlobalStatisticsManager::UnitVanished(Unit* unit)
	{
		uint8 owner = unit->GetOwner();
		_playersStats[owner]->UnitVanished(unit);
	}

	PlayerStatsManager::ScoreStats::ScoreStats() :
		KilledUnitsCount(0),
		LostUnitsCount(0),
		BuildUnitsCount(0),
		KilledStructuresCount(0),
		LostStructuresCount(0),
		BuildStructuresCount(0),
		ResourcesGathered(0),
		ResourcesSpent(0),
		TotalScore(0)
	{

	}

	PlayerStatsManager::UnitKindInfo::UnitKindInfo() :
		Units(11u, 2u),
		ConstructedCount(0)
	{ }
}