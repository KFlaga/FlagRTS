#include "DefaultStateSuppliers.h"
#include "SceneObjectStates.h"
#include "UnitStates.h"
#include "MapDecoratorStates.h"
#include "Unit.h"
#include "GameCamera.h"
#include "MapDecorator.h"
#include "DefaultMoveStrategies.h"
#include "MissleStates.h"
#include "Missle.h"

namespace FlagRTS
{
	void MapDecoratorNoPrespawnStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
		object->AddState(SceneObjectStates::Idle, xNew1(MapDecoratorIdleState, static_cast<MapDecorator*>(object)));
	}

	void GameCameraDefaultStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
		object->AddState(SceneObjectStates::Idle, xNew0(SceneObjectIdleState));
	}

	void UnitBaseStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
		object->AddState(SceneObjectStates::Idle, xNew1(UnitIdleState, static_cast<Unit*>(object)));
		object->AddState(SceneObjectStates::Dying, xNew1(UnitDyingState, static_cast<Unit*>(object)));
	}

	void UnitAggresiveStateSupplier::AddStatesToObject(SceneObject* object)
	{
		// object->AddState(SceneObjectStates::Idle, xNew1(UnitIdleState, static_cast<Unit*>(object)));
	}

	void UnitPassiveStateSupplier::AddStatesToObject(SceneObject* object)
	{
		// object->AddState(SceneObjectStates::Idle, xNew1(UnitIdleState, static_cast<Unit*>(object)));
	}

	void UnitMoveGroundStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::Move, xNew1(UnitMoveState, static_cast<Unit*>(object)));
		object->AddState(SceneObjectStates::ForceMove, xNew1(UnitForceMoveState, static_cast<Unit*>(object)));
	}

	void UnitRoteteableStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::ForceMove, xNew1(UnitForceMoveState, static_cast<Unit*>(object)));
	}

	void UnitAttackGroundStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::Attack, xNew1(UnitAttackState, static_cast<Unit*>(object)));
	}

	void BuildingStateSupplier::AddStatesToObject(SceneObject* object)
	{
		auto buildState = xNew1(BuildingUnderConstructionState, static_cast<Unit*>(object));
		object->AddState(SceneObjectStates::UnderConstruction, buildState);
	}

	void ConstructionSiteStateSupplier::AddStatesToObject(SceneObject* object)
	{
		auto buildState = xNew1(ConstructionSiteBuildingState, static_cast<Unit*>(object));
		object->AddState(SceneObjectStates::OnSpawn, buildState);
		object->AddState(SceneObjectStates::UnderConstruction, buildState);
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
	}

	void SceneMarkerDefaultStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
		object->AddState(SceneObjectStates::Idle, xNew0(SceneObjectIdleState));
	}

	void MissleDefaultStateSupplier::AddStatesToObject(SceneObject* object)
	{
		object->AddState(SceneObjectStates::NotSpawned, xNew0(SceneObjectNotSpawnedState));
		object->AddState(SceneObjectStates::Idle, xNew1(MissleIdleState, static_cast<Missle*>(object)));
		object->AddState(SceneObjectStates::Move, xNew1(MissleMoveState, static_cast<Missle*>(object)));
		object->AddState(SceneObjectStates::Dying, xNew1(MissleDyingState, static_cast<Missle*>(object)));
	}
}