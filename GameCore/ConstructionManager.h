#pragma once

#include "DataTypes.h"
#include <Array.h>
#include <Event.h>
#include <ArrayMap.h>

namespace PathFinding
{
	class UniformGridPathingMap;
}

namespace FlagRTS
{
	class Unit;
	class ConstructionPlanner;
	class ConstructionSiteFactory;
	struct PlanningResult;
	class UnitDefinition;

	// ConstructionManager is responsible for placing/constructing buildings :
	// - first when command is issued ( extrnally ) 'StartNewConstructionPlan' is called and
	// ConstructionPlanner is starting new plan - that is shows building outline on scene and user
	// can choose correct place for new building
	// For automated building (like for AI or queued orders) planning can be ommited
	// - when plan is choosen, ConstructionSiteFactory is used for creating ConstructionSite object
	// placed on planned position by calling BeginConstruction
	// - building is spawned and enters UnderConstruction state
	// - construction sites are updated till finished ( building logic is handled there )
	// - on constrcution finished ( event fired by ConstructionSite ), site is despawned and
	//	building ConstructionFinished event is fired, building enters idle state
	// - construction can be aborted by calling abort construction ( with construction site as param ) ->
	// site and building are despawned ( it should be called by firing BuildingAborted event in construction site )
	// Behaviour is customisable by changing planner and site factory or supplying custom
	// ObjectStateSuppliers for class ConstructionSite
	// Visual planing assumes it is done for client player
	class ConstructionManager
	{
	protected:
		Ogre::SceneManager* _ogreMgr;
		ArrayMaps<ConstructionPlanner*>::KeyCCString _plannerNamedMap;
		ConstructionPlanner* _planner;
		ConstructionSiteFactory* _siteFactory;
		Array<Unit*> _pendingSiteDeletes; // Delete finished csites not event-driven to avoid self-delete of csite

	public:
		ConstructionManager(Ogre::SceneManager* ogreMgr, 
			PathFinding::UniformGridPathingMap* obstaclesMap);
		~ConstructionManager();
		
		void Update(float ms);

		void StartNewConstructionPlan(UnitDefinition* plannedBuildingDef, 
			const Vector3& startPosition);
		void UpdatePlanPosition(const Vector3& terrainPosition);
		void AbandonPlan();
		const PlanningResult& AcceptPlan();
		const PlanningResult& GetLastPlanningResult() const;

		// Just checks of builiding fits given location, w/o visual planning
		// Additionaly owning player is required
		bool CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
			const Vector3& terrainPosition, uint8 player);

		// When begining construction it is assumed, that area is clear ( it should be
		// checked externally )
		void BeginConstruction(UnitDefinition* plannedBuildingDef, 
			const Vector3& terrainPosition, int owner);

		void AbortConstruction(Unit* site);

		ConstructionSiteFactory* GetConstructionSiteFactory() const { return _siteFactory; }
		void SetConstructionSiteFactory(ConstructionSiteFactory* factory);

		// Adds new planner with given name. Deletes old one if there was onr with this name registered already
		void AddConstructionPlanner(const char* name, ConstructionPlanner* planner);
		
		ConstructionPlanner* FindConstructionPlanner(const char* plannerName)
		{
			auto plannerIt = _plannerNamedMap.find(plannerName);
			if(plannerIt != _plannerNamedMap.end())
			{
				return plannerIt->Value;
			}
			return 0;
		}

		ConstructionPlanner* GetCurrentConstructionPlanner() const { return _planner; }

		// Changes planner to be used on next plans
		void ChangeConstructionPlanner(ConstructionPlanner* planner);
		void ChangeConstructionPlanner(const char* plannerName)
		{
			auto planner = FindConstructionPlanner(plannerName);
			_ASSERT( planner != 0 );
			if( planner == 0 )
				planner = _plannerNamedMap["Default"];
			ChangeConstructionPlanner( _plannerNamedMap[plannerName] );
		}

	protected:
		void FinishConstruction(Unit* site, Unit* building, bool aborted);

		DEFINE_DELEGATE3( ConstructionFinishedDelegate, 
			ConstructionManager, FinishConstruction, Unit*, Unit*, bool);
		ConstructionFinishedDelegate _onConstructionFinished;
	};
}