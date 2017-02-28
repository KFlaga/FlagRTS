#include "ConstructionManager.h"
#include "ConstructionPlanner.h"
#include "ConstructionSiteFactory.h"
#include "Unit.h"
#include "GameWorld.h"
#include "ISceneObjectSpawner.h"
#include "IGameObjectPool.h"

namespace FlagRTS
{
	ConstructionManager::ConstructionManager(Ogre::SceneManager* ogreMgr, 
		PathFinding::UniformGridPathingMap* obstaclesMap) :
	_ogreMgr(ogreMgr),
		_planner(xNew2(ConstructionPlanner, ogreMgr, obstaclesMap)),
		_siteFactory(xNew2(ConstructionSiteFactory, ogreMgr, obstaclesMap)),
		_onConstructionFinished(this)
	{
		AddConstructionPlanner("Default", _planner);
	}

	ConstructionManager::~ConstructionManager()
	{
		xDeleteSafe(_planner);
		xDeleteSafe(_siteFactory);
	}

	void ConstructionManager::Update(float ms)
	{
		for(unsigned int i = 0; i < _pendingSiteDeletes.size(); ++i)
		{
			_siteFactory->DestroySite(_pendingSiteDeletes[i]);
		}
		_pendingSiteDeletes.clear();

		_planner->Update(ms);
	}

	void ConstructionManager::StartNewConstructionPlan(UnitDefinition* plannedBuildingDef, 
		const Vector3& startPosition)
	{
		_planner->StartNewPlan(plannedBuildingDef, startPosition);
	}

	void ConstructionManager::UpdatePlanPosition(const Vector3& terrainPosition)
	{
		_planner->SetPlannedObjectPosition(terrainPosition);
	}

	void ConstructionManager::AbandonPlan()
	{
		_planner->AbandonPlan();
	}

	const PlanningResult& ConstructionManager::AcceptPlan()
	{
		return _planner->AcceptPlan();
	}

	const PlanningResult& ConstructionManager::GetLastPlanningResult() const
	{
		return _planner->GetLastPlanningResult();
	}

	bool ConstructionManager::CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
			const Vector3& terrainPosition, uint8 player)
	{
		return _planner->CheckPlanIsValid(plannedBuildingDef, terrainPosition, player);
	}

	void ConstructionManager::BeginConstruction(UnitDefinition* plannedBuildingDef, 
		const Vector3& terrainPosition, int owner)
	{
		Unit* building = static_cast<Unit*>(GameInterfaces::GetGameObjectPool()->
			Create(plannedBuildingDef, owner));

		// Set new commands to building -> that is only AbortBuildingConstructionCommand
		// Rest will be enabled after construction is finished

		Unit* csite = _siteFactory->CreateSite(building, owner);
		building->ConstructionFinished() += &_onConstructionFinished;

		GameInterfaces::GetSceneObjectSpawner()->SpawnSceneObject(csite, 
			SpawnInfo(Quaternion::IDENTITY, terrainPosition, false));
		// Building should be spawned from within ConstructionSite
	}

	void ConstructionManager::AbortConstruction(Unit* site)
	{
		// Aborting construction might occur for 2 reasons :
		// - user stopped building
		// - building  was destroyed
		Unit* building = _siteFactory->GetConstructedBuilding(site);

		// For now assume it was stopped
		// Things like resource return should be done from within ConstructionSite state
		// If it was aborted, just despawn and destroy both things
		GameInterfaces::GetSceneObjectSpawner()->DestroySceneObject(building);

		_pendingSiteDeletes.push_back(site);
	}

	void ConstructionManager::FinishConstruction(Unit* site, Unit* building, bool aborted)
	{
		if( aborted == true )
		{
			AbortConstruction(site);
			return;
		}

		GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(site);

		// If construction was finished, destory site
		_pendingSiteDeletes.push_back(site);
		building->ChangeState(SceneObjectStates::Idle);
	}

	void ConstructionManager::SetConstructionSiteFactory(ConstructionSiteFactory* factory)
	{
		xDeleteSafe(_siteFactory);
		_siteFactory = factory;
	}

	void ConstructionManager::AddConstructionPlanner(const char* name, ConstructionPlanner* planner)
	{
		ConstructionPlanner* oldplanner = FindConstructionPlanner(name);
		_plannerNamedMap[name] = planner;
		if(oldplanner != 0)
			xDelete(oldplanner);
	}

	void ConstructionManager::ChangeConstructionPlanner(ConstructionPlanner* planner)
	{
		_planner = planner;
	}
}