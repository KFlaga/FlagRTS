#pragma once

#include <ConstructionPlanner.h>
#include <Event.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class ModelMarker;
	class ModelMarkerDefinition;
	class Unit;

	// Extnsion of default construction planner :
	// for building to be build it must be within LogisticsCenter operational area
	// When planning it draws circles around each LC for some visual help
	// TODO : also add requirement for building operations to be within range
	class LogisticCenterInRangePlanner : public ConstructionPlanner
	{
	protected:
		size_t _logisticsCenterKind;
		ModelMarkerDefinition* _logisticsCenterRangeMarkerDef;
		ArrayMap<Unit*, ModelMarker*> _lcRangeMarkers;
		uint8 _client;

	public:
		LogisticCenterInRangePlanner(Ogre::SceneManager* ogreMgr, 
			PathFinding::UniformGridPathingMap* obstacleMap);
		~LogisticCenterInRangePlanner();

		void Update(float ms);

		void SetPlannedObjectPosition(const Vector3& terrainPosition);
		void AbandonPlan();
		void StartNewPlan(UnitDefinition* plannedBuildingDef, const Vector3& startPosition);

		bool CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
			const Vector3& terrain, uint8 player);

	protected:
		void OnUnitKilled(Unit* unit, uint8 owner);
		DEFINE_DELEGATE2(UnitKilledDelegate, LogisticCenterInRangePlanner,
			OnUnitKilled, Unit*, uint8);
		UnitKilledDelegate _onUnitKilled;

		void OnUnitConstructed(Unit* unit, uint8 owner);
		DEFINE_DELEGATE2(UnitConstructedDelegate, LogisticCenterInRangePlanner,
			OnUnitConstructed, Unit*, uint8);
		UnitConstructedDelegate _onUnitConstructed;
	};
}