#include "LogisticsCenterInRangePlanner.h"
#include <ObjectOutline.h>
#include <Unit.h>
#include <PathFindingUniformGridPathingMap.h>
#include <GameWorld.h>
#include <Map.h>

#include <Ogre\OgreSceneManager.h>
#include <CellColorer.h>
#include <PlayerStatsManager.h>
#include "LogisticsCenterData.h"
#include <ModelMarker.h>
#include <ModelMarkerDefinition.h>
#include <Player.h>

namespace FlagRTS
{	
	LogisticCenterInRangePlanner::LogisticCenterInRangePlanner(Ogre::SceneManager* ogreMgr, 
		PathFinding::UniformGridPathingMap* obstacleMap) :
	ConstructionPlanner(ogreMgr, obstacleMap),
		_onUnitKilled(this),
		_onUnitConstructed(this)
	{
		_logisticsCenterKind = static_cast<UnitDefinition*>(GameWorld::GlobalWorld->
			GetSceneObjectDefinition("Unit", "LogisticsCenter"))->GetUnitKind();
		_logisticsCenterRangeMarkerDef = static_cast<ModelMarkerDefinition*>(
			GameWorld::GlobalWorld->GetSceneObjectDefinition("ModelMarker", "LCRangeCircle"));
		_client = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();

		GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(_client)->GetUnitConstructedEvent() += &_onUnitConstructed;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(_client)->GetUnitKilledEvent() += &_onUnitKilled;
	}

	LogisticCenterInRangePlanner::~LogisticCenterInRangePlanner()
	{
		GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(_client)->GetUnitConstructedEvent() -= &_onUnitConstructed;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(_client)->GetUnitKilledEvent() -= &_onUnitKilled;
	}

	void LogisticCenterInRangePlanner::Update(float ms)
	{
		ConstructionPlanner::Update(ms);
	}

	void LogisticCenterInRangePlanner::SetPlannedObjectPosition(const Vector3& terrainPosition)
	{
		ConstructionPlanner::SetPlannedObjectPosition(terrainPosition);
		if( _result.IsLocationValid == false )
			return;

		// In addition to default obstacle check also check if there is
		// logistics center near
		auto allLCs = GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(_client)->GetAllUnitsOfKind(_logisticsCenterKind);

		_result.IsLocationValid = false;
		// Iterate over them and if some is within range its ok, otherwise plan failed
		for(auto lcIt = allLCs->begin(); lcIt != allLCs->end(); ++lcIt)
		{
			if( lcIt->Value->IsUnderConstruction() == false &&
				lcIt->Value->GetPositionAbsolute().squaredDistance(terrainPosition) <=
				lcIt->Value->GetObjectSpecificData<LogisticsCenterObjectData>()->CurrentSquaredInfluenceRadius )
			{
				_result.IsLocationValid = true;
				break;
			}
		}

		if(_result.IsLocationValid == false)
		{
			_currentOutline->SetIsStateValid(false);
		}
	}

	bool LogisticCenterInRangePlanner::CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
		const Vector3& terrainPosition, uint8 player)
	{
		// Get all LCs
		auto allLCs = GameWorld::GlobalWorld->GetPlayerStatsManager()->
			GetPlayerStats(player)->GetAllUnitsOfKind(_logisticsCenterKind);

		// Iterate over them and if some is within range its ok, otherwise plan failed
		for(auto lcIt = allLCs->begin(); lcIt != allLCs->end(); ++allLCs)
		{
			if( lcIt->Value->IsUnderConstruction() == false &&
				lcIt->Value->GetPositionAbsolute().squaredDistance(terrainPosition) <=
				lcIt->Value->GetObjectSpecificData<LogisticsCenterObjectData>()->CurrentSquaredInfluenceRadius )
			{
				return ConstructionPlanner::CheckPlanIsValid(plannedBuildingDef, terrainPosition, player);
			}
		}

		return false;
	}

	void LogisticCenterInRangePlanner::AbandonPlan()
	{
		// despawn range indicators
		for(auto markerIt = _lcRangeMarkers.begin(); markerIt != _lcRangeMarkers.end(); ++markerIt)
		{
			GameWorld::GlobalWorld->DespawnSceneObject(markerIt->Value);
		}
		ConstructionPlanner::AbandonPlan();
	}

	void LogisticCenterInRangePlanner::StartNewPlan(UnitDefinition* plannedBuildingDef, 
		const Vector3& startPosition)
	{
		ConstructionPlanner::StartNewPlan(plannedBuildingDef, startPosition);
		// Spawn all LCs influence area markers
		for(auto markerIt = _lcRangeMarkers.begin(); markerIt != _lcRangeMarkers.end(); ++markerIt)
		{
			// Markers should be already created
			ModelMarker* marker = markerIt->Value;

			float radius = markerIt->Key->GetObjectSpecificData<LogisticsCenterObjectData>()->CurrentInfluenceRadius;
			
			GameWorld::GlobalWorld->SpawnSceneObject(marker, 
				SpawnInfo( Quaternion::IDENTITY, markerIt->Key->GetPositionAbsolute() + Vector3(0.f,20.f,0.f)));
			marker->SetSize(Vector3(radius*2.f, 0.5f, radius*2.f));
		}
	}

	void LogisticCenterInRangePlanner::OnUnitKilled(FlagRTS::Unit* unit, uint8 owner)
	{
		// If it was clients LC remove its area marker
		if(unit->GetUnitKind() == _logisticsCenterKind)
		{
			auto markerIt = _lcRangeMarkers.find(unit);
			if(markerIt != _lcRangeMarkers.end())
			{
				// If planner is planning also despawn it
				if( IsPlaning() )
					GameWorld::GlobalWorld->DespawnSceneObject(markerIt->Value);
				GameWorld::GlobalWorld->DestroySceneObject(markerIt->Value);
				_lcRangeMarkers.erase(markerIt);
			}
		}
	}

	void LogisticCenterInRangePlanner::OnUnitConstructed(Unit* unit, uint8 owner)
	{
		// If it was clients LC add its area marker
		if(unit->GetUnitKind() == _logisticsCenterKind)
		{
			ModelMarker* marker = static_cast<ModelMarker*>(
				GameWorld::GlobalWorld->CreateSceneObject(_logisticsCenterRangeMarkerDef, 
				NEUTRAL_PLAYERNUM));

			// If planner is planning also spawn it
			if( IsPlaning())
			{
				float radius = unit->GetObjectSpecificData<LogisticsCenterObjectData>()->CurrentInfluenceRadius;
				
				GameWorld::GlobalWorld->SpawnSceneObject(marker, 
					SpawnInfo( Quaternion::IDENTITY, unit->GetPositionAbsolute() + Vector3(0.f,10.f,0.f)));
				marker->SetSize(Vector3(radius*2.f, 0.5f, radius*2.f));
			}

			_lcRangeMarkers[unit] = marker;
		}
	}
}