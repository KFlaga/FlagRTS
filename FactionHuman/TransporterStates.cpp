#include "TransporterStates.h"
#include "MetalMineOrders.h"
#include "MetalMineData.h"
#include "TransporterData.h"
#include "TransporterOrders.h"
#include "ResourceStoreData.h"
#include "ResourceStoreOrders.h"
#include "PackageData.h"
#include <Unit.h>
#include <GameWorld.h>
#include <PathingSystem.h>
#include "EcoResources.h"
#include <PlayerStatsManager.h>
#include <AnimationController.h>


namespace FlagRTS
{
	DllExclusive TransporterIdleState::TransporterIdleState(Unit* owner) :
		UnitIdleState(owner)
	{
		
	}

	DllExclusive void TransporterIdleState::Begin()
	{
		UnitIdleState::Begin();
		TransporterTypeData* typeData = _owner->GetKindSpecificData<TransporterTypeData>();
		_autoMineTimer = typeData->AutoMineTime;
	}

	DllExclusive void TransporterIdleState::End()
	{
		UnitIdleState::End();
	}

	DllExclusive void TransporterIdleState::Update(float ms)
	{
		TransporterTypeData* typeData = _owner->GetKindSpecificData<TransporterTypeData>();
		if(typeData->AutoLookForMine)
		{
			_autoMineTimer -= ms;
			if(_autoMineTimer <= 0.f)
			{
				_owner->ChangeState(GetTypeId<TransporterPickingUpPackageState>());
				return;
			}
		}
		UnitIdleState::Update(ms);
	}


	DllExclusive TransporterWaitingForMetalMineToBeFreedState::TransporterWaitingForMetalMineToBeFreedState(Unit* owner) :
		UnitState(owner)
	{ }

	// In this state unit is commandable unless theres free spot on mine
	// Transporter is near metal mine drive-in, but mine is currently occupied
	// It just waits for free spot, occupies ot and moves to DriveInSpot precisely
	DllExclusive void TransporterWaitingForMetalMineToBeFreedState::Begin()
	{
		_waitingForSpot = true;
	}

	DllExclusive void TransporterWaitingForMetalMineToBeFreedState::End()
	{

	}

	DllExclusive void TransporterWaitingForMetalMineToBeFreedState::Update(float ms)
	{
		if(_waitingForSpot == true)
		{
			TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
			auto mineData = transporterData->TargetMine->GetObjectSpecificData<MetalMineObjectData>();
			if( mineData->Transporter == 0)
			{
				// Mine is free, so reserve it
				mineData->Transporter = _owner;
				_waitingForSpot = false;
				_owner->UnitDied() += &mineData->OnTransporterDied;

				_owner->ChangeState(GetTypeId<TransporterMovingIntoMetalMineDrivewayState>());
				return;
			}
		}
		UnitState::Update(ms);
	}

	DllExclusive TransporterMovingIntoMetalMineDrivewayState::TransporterMovingIntoMetalMineDrivewayState(Unit* owner) :
		UnitState(owner)
	{

	}

	DllExclusive void TransporterMovingIntoMetalMineDrivewayState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Move, "Default");
		// Mine should be occupied by _owner
		// Disallow unit pathing and commands
		_owner->SetIsInteractable(false);
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, true);
		// TODO :
		// _targetMine->UnitDied() += _onMineDied;

		_rotated = false;
		_onSpot = false;
	}

	DllExclusive void TransporterMovingIntoMetalMineDrivewayState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void TransporterMovingIntoMetalMineDrivewayState::Update(float ms)
	{
		// Such a update :
		// Rotate to 'minerotation'
		// Play animation (Internal,MineDriveIn)
		// Wait for animation end

		if(_rotated)
		{
			// We know that anims[0] should be MineDriveIn
			// More proper would be FindAnimation(SceneObjectStates::Internal, "MineDriveIn")
			if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
			{
				_owner->ChangeState(GetTypeId<TransporterWaitingForMetalMinePackageState>());
			}
		}
		else if(_onSpot) // -> only rotation left
		{
			TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();

			// Rotate to target with half the rotation speed
			auto targetDirection = transporterData->TargetMine->GetDirectionAbsolute();
			targetDirection.y = 0.f;
			auto currentDirection = _owner->GetDirectionAbsolute();
			currentDirection.y = 0.f;
			auto angleToTargetDir = currentDirection.angleBetween(targetDirection);
			int sign = -targetDirection.x*currentDirection.z + 
				targetDirection.z*currentDirection.x > 0 ? -1 : 1;

			Radian maxRot(Degree(_owner->GetRotationSpeed() * 0.5f * ms));

			if( maxRot >= angleToTargetDir ) // If max rot is bigger just rotate needed angle and we're done
			{
				_owner->SetOrientationQuiet(transporterData->TargetMine->GetOrientation());
				ms -= (maxRot - angleToTargetDir).valueDegrees() / (_owner->GetRotationSpeed() * 0.5f);

				_rotated = true;
				_owner->GetAnimController().ChangeAnimation(
					SceneObjectStates::Internal, "MineDriveIn", false);
				_owner->GetAnimController().AddAnimation(
					SceneObjectStates::Move, "Default");

				Update(ms);
			}
			else
			{
				_owner->Rotate(Vector3(0.0f,sign*1.0f,0.0f), maxRot);
			}
		}
		else
		{
			// Transporter needs to move to close precisely
			TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
			Vector3 finalSpot = transporterData->TargetMine->
				GetKindSpecificData<MetalMineTypeData>()->TransporterDriveInSpot;

			finalSpot = transporterData->TargetMine->GetPositionAbsolute() + 
				transporterData->TargetMine->GetOrientation() * finalSpot;
			Vector3 oldPos = _owner->GetPositionAbsolute();
			oldPos.y = 0.f;
			_owner->MoveUnit(finalSpot, ms);
			if( _owner->GetMoveStrategy()->GetRemainingSquaredDistance() < 1.0f )
			{
				_owner->SetPositionQuiet(finalSpot);
				_onSpot = true;
				// Get remaining time and update again
				ms -= finalSpot.distance(oldPos) / _owner->GetSpeed();
				if(ms > 1.f)
					Update(ms);
			}
		}
		UnitState::Update(ms);
	}

	DllExclusive TransporterWaitingForMetalMinePackageState::TransporterWaitingForMetalMinePackageState(Unit* owner) :
		UnitState(owner)
	{

	}

	DllExclusive void TransporterWaitingForMetalMinePackageState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "Default");

		// Inform mine that transporter is in place, so it can begin package load
		TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
		CommandTarget target;
		target.SetTargetUnit(_owner);
		transporterData->TargetMine->ExecuteCommand( GetTypeId<LoadPackageToTransporterCommand>(), &target);
	}

	DllExclusive void TransporterWaitingForMetalMinePackageState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void TransporterWaitingForMetalMinePackageState::Update(float ms)
	{
		UnitState::Update(ms);
		// Just wait for mine to order transporter to leave its driveway
	}

	DllExclusive TransporterLeavingMetalMineState::TransporterLeavingMetalMineState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterLeavingMetalMineState::Begin()
	{
		// Metal mine just ordered transorter to leave or user ordered it
		// Actually we dont care much here, just play the animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "MineDriveOut", false);
		_owner->GetAnimController().AddAnimation(
			SceneObjectStates::Move, "Default");
	}

	DllExclusive void TransporterLeavingMetalMineState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		// Make unit commandable again
	}

	DllExclusive void TransporterLeavingMetalMineState::Update(float ms)
	{
		UnitState::Update(ms);
		// We know that anims[0] should be MineDriveOut
		// More proper would be FindAnimation(SceneObjectStates::Internal, "MineDriveOut")
		if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
		{
			// Free mine slot
			TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
			auto mineData = transporterData->TargetMine->GetObjectSpecificData<MetalMineObjectData>();
			mineData->Transporter = 0;
			_owner->UnitDied() -= &mineData->OnTransporterDied;
			_owner->ChangeState(GetTypeId<TransporterDeliveringPackageState>());
		}
	}

	DllExclusive TransporterDeliveringPackageState::TransporterDeliveringPackageState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterDeliveringPackageState::Begin()
	{
		// If unit have a store specified, move to it
		// If unit have no store specified, then look for closest one
		TransporterObjectData* objectData = _owner->GetObjectSpecificData<TransporterObjectData>();
		auto playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_owner->GetOwner());
		_owner->ChangeState(SceneObjectStates::Idle);

		if(objectData->TargetStore == 0)
		{
			// Find closest store
			Unit* store = playerStats->FindClosestUnitOfKind(_owner->GetPositionAbsolute(), 
				static_cast<UnitDefinition*>(GameWorld::GlobalWorld->
				GetSceneObjectDefinition("Unit", "Storage"))->GetUnitKind());

			if( store != 0 )
			{
				objectData->TargetStore = store;
				CommandTarget ct;
				ct.SetTargetUnit(objectData->TargetStore);
				_owner->ExecuteCommand(OrderTypes::ReturnResources, &ct);	
			}
			// else : No store on map -> do nothing
		}
		else
		{
			// Check if store is player choosen, if yes go to it, otherwise
			// check if there's closer one
			if(objectData->IsStorePlayerChoosen == false)
			{
				Unit* store = playerStats->FindClosestUnitOfKind(_owner->GetPositionAbsolute(), 
					objectData->TargetStore->GetUnitKind());
				objectData->TargetStore = store;
			}

			// Order deliver to mine
			CommandTarget ct;
			ct.SetTargetUnit(objectData->TargetStore);
			_owner->ExecuteCommand(OrderTypes::ReturnResources, &ct);
		}
	}

	DllExclusive void TransporterDeliveringPackageState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void TransporterDeliveringPackageState::Update(float ms)
	{
		UnitState::Update(ms);
	}

	DllExclusive TransporterLeavingMetalMineNoPackageState::TransporterLeavingMetalMineNoPackageState(Unit* owner) :
		UnitState(owner)
	{ }


	DllExclusive void TransporterLeavingMetalMineNoPackageState::Begin()
	{

	}

	DllExclusive void TransporterLeavingMetalMineNoPackageState::End()
	{

	}

	DllExclusive void TransporterLeavingMetalMineNoPackageState::Update(float ms)
	{

	}

	DllExclusive TransporterWaitingForResourceStoreToBeFreedState::
		TransporterWaitingForResourceStoreToBeFreedState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterWaitingForResourceStoreToBeFreedState::Begin()
	{
		_waitingForSpot = true;
	}

	DllExclusive void TransporterWaitingForResourceStoreToBeFreedState::End()
	{

	}

	DllExclusive void TransporterWaitingForResourceStoreToBeFreedState::Update(float ms)
	{
		if(_waitingForSpot == true)
		{
			TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
			auto storeData = transporterData->TargetStore->GetObjectSpecificData<ResourceStoreObjectData>();
			if( storeData->Transporter == 0)
			{
				// Store is free, so reserve it
				storeData->Transporter = _owner;
				_owner->UnitDied() += &storeData->OnTransporterDied;
				_waitingForSpot = false;
				// Order store to open its doors
				CommandTarget ct;
				ct.SetTargetUnit(_owner);
				transporterData->TargetStore->ExecuteCommand(
					GetTypeId<StoreOpenDoorsCommand>(), &ct);

				_owner->ChangeState(GetTypeId<TransporterMovingIntoStoreState>());
				return;
			}
		}
		UnitState::Update(ms);
	}

	// 1) Wait for store doors to be open and move to store spot simultaneously
	// 2) If reached the spot, rotate to -storeDirection
	// 3) If store doors are not opened yet, wait for it
	// 4) If store doors are opened play drive in animation
	// 5) On anim finish order to close door and change state to TransporterInStore
	DllExclusive TransporterMovingIntoStoreState::TransporterMovingIntoStoreState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterMovingIntoStoreState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Move, "Default");
		// Mine should be occupied by _owner
		// Disallow unit pathing and commands
		_owner->SetIsInteractable(false);
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, true);
		// TODO :
		// _targetMine->UnitDied() += _onMineDied;

		_rotated = false;
		_onSpot = false;
		_doorsOpened = false;
	}

	DllExclusive void TransporterMovingIntoStoreState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void TransporterMovingIntoStoreState::Update(float ms)
	{
		TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
		if(_doorsOpened)
		{
			// We know that anims[0] should be MineDriveIn
			// More proper would be FindAnimation(SceneObjectStates::Internal, "StoreDriveIn")
			if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
			{
				_owner->ChangeState(GetTypeId<TransporterInStoreStoreState>());
			}
		}
		else if(_rotated)
		{
			// Wait for doors
			if(transporterData->TargetStore->
				GetObjectSpecificData<ResourceStoreObjectData>()->DoorsOpened)
			{
				_doorsOpened = true;
				// Play drive in animation
				_owner->GetAnimController().ChangeAnimation(
					SceneObjectStates::Internal, "StoreDriveIn", false);
				_owner->GetAnimController().AddAnimation(
					SceneObjectStates::Move, "Default");
			}
		}
		else if(_onSpot) // -> only rotation left
		{
			// Rotate to target with half the rotation speed ( direction opposite to store )
			auto targetDirection = -transporterData->TargetStore->GetDirectionAbsolute();
			targetDirection.y = 0.f;
			auto currentDirection = _owner->GetDirectionAbsolute();
			currentDirection.y = 0.f;
			auto angleToTargetDir = currentDirection.angleBetween(targetDirection);
			int sign = -targetDirection.x*currentDirection.z + 
				targetDirection.z*currentDirection.x > 0 ? -1 : 1;

			Radian maxRot(Degree(_owner->GetRotationSpeed() * 0.5f * ms));

			if( maxRot >= angleToTargetDir ) // If max rot is bigger just rotate needed angle and we're done
			{
				_owner->Rotate(Vector3(0.0f,sign*1.0f,0.0f), angleToTargetDir);
				ms -= (maxRot - angleToTargetDir).valueDegrees() / (_owner->GetRotationSpeed() * 0.5f);

				_doorsOpened = false;
				_rotated = true;
				Update(ms);
			}
			else
			{
				_owner->Rotate(Vector3(0.0f,sign*1.0f,0.0f), maxRot);
			}
		}
		else
		{
			// Transporter needs to move to close precisely
			Vector3 finalSpot = transporterData->TargetStore->
				GetKindSpecificData<ResourceStoreTypeData>()->DoorSpot;

			finalSpot = transporterData->TargetStore->GetPositionAbsolute() + 
				transporterData->TargetStore->GetOrientation() * finalSpot;
			Vector3 oldPos = _owner->GetPositionAbsolute();
			oldPos.y = 0.f;
			_owner->MoveUnit(finalSpot, ms);
			if( _owner->GetMoveStrategy()->GetRemainingSquaredDistance() < 1.0f )
			{
				_owner->SetPositionQuiet(finalSpot);
				_onSpot = true;
				// Get remaining time and update again
				ms -= finalSpot.distance(oldPos) / _owner->GetSpeed();
				if(ms > 1.f)
					Update(ms);
			}
		}
		UnitState::Update(ms);
	}

	// Transporter reached the store, so request door closure
	// When doors are closed: remove package, add resources, open doors
	// When doors are opened play move out animation
	// When moven out -> if there's mine set, move to mine to pick up next package

	DllExclusive TransporterInStoreStoreState::
		TransporterInStoreStoreState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterInStoreStoreState::Begin()
	{
		TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
		CommandTarget ct;
		transporterData->TargetStore->ExecuteCommand(
			GetTypeId<StoreCloseDoorsCommand>(), &ct);

		_waitingForDoorClosed = true;
		_waitingForDoorOpened = false;
	}

	DllExclusive void TransporterInStoreStoreState::End()
	{

	}

	DllExclusive void TransporterInStoreStoreState::Update(float ms)
	{
		TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
		ResourceStoreObjectData* storeData = transporterData->TargetStore->
			GetObjectSpecificData<ResourceStoreObjectData>();
		if(_waitingForDoorClosed == true)
		{
			if( storeData->DoorsClosed == true )
			{
				_waitingForDoorClosed = false;
				// Add resources
				int resAmount = transporterData->Package->
					GetObjectSpecificData<PackageObjectData>()->StoredMetal;
				GameWorld::GlobalWorld->GetResources()->AddResourceToPlayer(
					_owner->GetOwner(), EcoResources::Metal, resAmount);

				// Detach and destory package
				_owner->DetachChild(transporterData->Package, true, true);
				GameWorld::GlobalWorld->DespawnSceneObject(transporterData->Package);
				GameWorld::GlobalWorld->DestroySceneObject(transporterData->Package);

				// Update data
				transporterData->Package = 0;

				_waitingForDoorOpened = true;
				CommandTarget ct;
				transporterData->TargetStore->ExecuteCommand(
					GetTypeId<StoreOpenDoorsCommand>(), &ct);
			}
			return;
		}
		else if(_waitingForDoorOpened)
		{
			if( storeData->DoorsOpened == true )
			{
				// Rotate 180 degs and move out
				_owner->Rotate(Vector3(0.f,1.f,0.f), Degree(180.f));
				_owner->ChangeState(GetTypeId<TransporterMovingOutOfStoreState>());
			}
		}
		UnitState::Update(ms);
	}

	DllExclusive TransporterMovingOutOfStoreState::
		TransporterMovingOutOfStoreState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterMovingOutOfStoreState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "StoreDriveOut", false);
		_owner->GetAnimController().AddAnimation(
			SceneObjectStates::Move, "Default");
	}

	DllExclusive void TransporterMovingOutOfStoreState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void TransporterMovingOutOfStoreState::Update(float ms)
	{
		TransporterObjectData* transporterData = _owner->GetObjectSpecificData<TransporterObjectData>();
		if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
		{
			// Close the doors behind
			CommandTarget ct;
			transporterData->TargetStore->ExecuteCommand(
				GetTypeId<StoreCloseDoorsCommand>(), &ct);
			// Free the store
			auto storeData = transporterData->TargetStore->
				GetObjectSpecificData<ResourceStoreObjectData>();
			storeData->Transporter = 0;
			_owner->UnitDied() -= &storeData->OnTransporterDied;

			// Same like delivering package state, but with mine, 
			// so maybe PickingUpPackage ??
			_owner->ChangeState(GetTypeId<TransporterPickingUpPackageState>());
		}
		UnitState::Update(ms);
	}

	DllExclusive TransporterPickingUpPackageState::TransporterPickingUpPackageState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void TransporterPickingUpPackageState::Begin()
	{
		// If unit have a store specified, move to it
		// If unit have no store specified, then look for closest one
		TransporterObjectData* objectData = _owner->GetObjectSpecificData<TransporterObjectData>();
		auto playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_owner->GetOwner());

		if(objectData->TargetMine == 0)
		{
			// Find closest mine
			Unit* mine = playerStats->FindClosestUnitOfKind(_owner->GetPositionAbsolute(), 
				static_cast<UnitDefinition*>(GameWorld::GlobalWorld->
				GetSceneObjectDefinition("Unit", "Mine"))->GetUnitKind());

			if( mine != 0 )
			{
				objectData->TargetMine = mine;
				CommandTarget ct;
				ct.SetTargetUnit(objectData->TargetMine);
				_owner->ExecuteCommand(OrderTypes::Gather, &ct);
			}
			// else : No store on map -> do nothing
		}
		else
		{
			// Order deliver to mine
			CommandTarget ct;
			ct.SetTargetUnit(objectData->TargetMine);
			_owner->ExecuteCommand(OrderTypes::Gather, &ct);
		}
		_owner->ChangeState(SceneObjectStates::Idle);
	}

	DllExclusive void TransporterPickingUpPackageState::End()
	{
		
	}

	DllExclusive void TransporterPickingUpPackageState::Update(float ms)
	{
		UnitState::Update(ms);
	}
}