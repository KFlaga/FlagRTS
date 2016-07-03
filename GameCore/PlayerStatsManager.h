#pragma once

#include "DataTypes.h"
#include <Array.h>
#include <HashMap.h>
#include <Event.h>

namespace FlagRTS
{
	class Unit;
	class UnitDefinition;
	// class Research;

	// Container for all tracked player statistics :
	// - build units
	// - researched technologies
	// - number of killed/losed unit etc
	class PlayerStatsManager
	{
	public:
		typedef HashedMap<size_t, Unit*> UnitBank;
		// Info on units of kind owned by player - for requirement checks mostly
		// contains additional field with constructed units count
		struct UnitKindInfo
		{
			UnitBank Units;
			int ConstructedCount;

			UnitKindInfo();
		};

		typedef HashedMap<size_t, UnitKindInfo*> UnitKindMap;
		// typedef Array<Research*> ResearchMap;

		struct ScoreStats
		{
			int KilledUnitsCount;
			int LostUnitsCount;
			int BuildUnitsCount;
			int KilledStructuresCount;
			int LostStructuresCount;
			int BuildStructuresCount;
			int ResourcesGathered;
			int ResourcesSpent;

			int TotalScore;

			ScoreStats();
		};

	private:
		UnitKindMap _ownedUnits; // All units owned by player
		ScoreStats _score;

		uint8 _player; // Player this data is about

		Event<Unit*, uint8> _unitSpawned;
		Event<Unit*, uint8> _unitConstructed;
		Event<Unit*, uint8> _unitKilled;

	public:
		PlayerStatsManager(uint8 player);
		~PlayerStatsManager();

		ScoreStats& GetScore() 
		{
			return _score;
		}

		UnitKindMap& GetAllOwnedUnits()
		{
			return _ownedUnits;
		}

		UnitKindInfo* GetUnitKindInfo(size_t kind)
		{
			return _ownedUnits[kind];
		}

		int GetUnitKindCount(size_t kind)
		{
			auto ukindIt = _ownedUnits.find(kind);
			if(ukindIt != _ownedUnits.end())
			{
				return ukindIt->Value->ConstructedCount;
			}
			return 0;
		}

		UnitBank* GetAllUnitsOfKind(size_t kind)
		{
			auto ukindIt = _ownedUnits.find(kind);
			if(ukindIt != _ownedUnits.end())
			{
				return &ukindIt->Value->Units;
			}
			return 0;
		}

		Unit* FindClosestUnitOfKind(const Vector3& point, size_t kind);

		void UnitSpawned(Unit* unit);
		void UnitKilled(Unit* unit);
		void UnitVanished(Unit* unit);

		void AddConstructedUnitScore(Unit* unit);
		void AddConstructedStructureScore(Unit* unit);
		void AddKilledUnitScore(Unit* unit);
		void AddKilledStructureScore(Unit* unit);
		void AddLostUnitScore(Unit* unit);
		void AddLostStructureScore(Unit* unit);

		Event<Unit*, uint8>& GetUnitSpawnedEvent() { return _unitSpawned; }
		Event<Unit*, uint8>& GetUnitConstructedEvent() { return _unitConstructed; }
		Event<Unit*, uint8>& GetUnitKilledEvent() { return _unitKilled; }

	private:
		// When building is spawned ( so added to player owned units ) it still needs to be
		// constructed to count, so add event listener 
		// It does not remove unit if it was aborted - it should be done when unit dies ( buildings on abort die )
		void UnitConstructionFinished(Unit* builder, Unit* building, bool wasAborted);
		DEFINE_DELEGATE3( ConstructionFinishedDelegate, 
			PlayerStatsManager, UnitConstructionFinished, Unit*, Unit*, bool);
		ConstructionFinishedDelegate _onConstructionFinished;
	};

	// Holds statistics for all players
	class GlobalStatisticsManager
	{
	private:
		Array<PlayerStatsManager*> _playersStats;

	public:
		GlobalStatisticsManager(uint8 playersCount);
		~GlobalStatisticsManager();

		PlayerStatsManager* GetPlayerStats(uint8 player)
		{
			return _playersStats[player];
		}

		// Called by GameWorld on unit spawn
		// saves it in its owner unit bank as exisiting + adds event handler OnConstructed if its building etc.
		void UnitSpawned(Unit* unit);
		// Called when unit dies -> updates player units + updates score ( so reason for
		// unit death should be passed too )
		void UnitKilled(Unit* unit);
		// Unit was despawned by any other mean than dying ( so OnDied was not called )
		void UnitVanished(Unit* unit);
	};
}