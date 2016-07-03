#include "MetalMineStates.h"
#include "PackageStates.h"
#include "TransporterOrders.h"
#include "MetalMineData.h"
#include "TransporterData.h"
#include <Unit.h>
#include "PackageData.h"
#include <GameWorld.h>
#include <AnimationController.h>

namespace FlagRTS
{
	DllExclusive MinePackageReadyState::MinePackageReadyState(Unit* owner) : 
		UnitState(owner)
	{ }

	DllExclusive void MinePackageReadyState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "Default");
	}

	DllExclusive void MinePackageReadyState::End()
	{
		
	}

	DllExclusive void MinePackageReadyState::Update(float ms)
	{
		UnitState::Update(ms);
		// If transporter gets near place a strong repulsive field on the entrance
		// or mark it as obstacle ( but this will prevent pathfinding, so
		// a repulive field is better )
	}

	DllExclusive MineLoadingPackageToTransporterState::MineLoadingPackageToTransporterState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void MineLoadingPackageToTransporterState::Begin()
	{
		// Start 'PickUpPackage' animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "PickUpPackage", false);
		_packagePicked = false;
		_packageHandedOver = false;

		// _targetTransporter->UnitDied() += abort loading
	}

	DllExclusive void MineLoadingPackageToTransporterState::End()
	{
		// Dont end animation here, as it can be moved back by Packageless return
	}

	DllExclusive void MineLoadingPackageToTransporterState::Update(float ms)
	{
		UnitState::Update(ms);

		// Check if current animation ended
		if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
		{
			// If ended check how much ms it is after end
			Animation* anim = _owner->GetAnimController().GetCurrentAnimations()[0];
			float overMs = std::max(0.f, anim->GetLength() * (anim->GetProgress() - 1.0f));

			if(_packagePicked == false)
				OnPackagePickedUp(overMs);
			else if(_packageHandedOver == false)
				OnPackageHandedOver(overMs);
			else
				OnCraneReturned(overMs);
		}
	}

	DllExclusive void MineLoadingPackageToTransporterState::OnPackagePickedUp(float overms)
	{
		// Pick-up package animation just ended
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "TransportPackage", false);
		// Also change package state to 'BeingTranspotedByCrane' and also update
		MetalMineObjectData* metalMineData = _owner->GetObjectSpecificData<MetalMineObjectData>();
		metalMineData->Package->ChangeState(GetTypeId<PackageBeingTransportedByMineCraneState>());
		//metalMineData->Package->Update(overms);

		_packagePicked = true;

		// Update with those overMs to ensure consistency
		//Update(overms);
	}

	DllExclusive void MineLoadingPackageToTransporterState::OnPackageHandedOver(float overms)
	{
		// 'TransportPackage' animation just ended : change package parent to transporter
		// and move it to transporter spot. Start 'ReturnCrane' animation

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "ReturnCrane", false);
		// Also change package state to 'BeingTranspotedByCrane' and also update
		MetalMineObjectData* metalMineData = _owner->GetObjectSpecificData<MetalMineObjectData>();
		metalMineData->Package->ChangeState(SceneObjectStates::Idle);
		metalMineData->Package->Update(overms);

		_owner->DetachChild(metalMineData->Package, true, false);
		metalMineData->PackageReady = false;

		metalMineData->Transporter->AttachChild(metalMineData->Package);
		metalMineData->Package->SetPosition(
			metalMineData->Transporter->GetKindSpecificData<TransporterTypeData>()->TransporterPackageSpot);
		metalMineData->Package->Rotate(Vector3(0.f,1.f,0.f), Degree(90.f));
		_packageHandedOver = true;

		auto transporterData = metalMineData->Transporter->GetObjectSpecificData<TransporterObjectData>();
		transporterData->Package = metalMineData->Package;
		metalMineData->Package = 0;

		// Order transporter to leave mine now
		CommandTarget ct;
		ct.SetTargetUnit(_owner);
		metalMineData->Transporter->GetCommandQueue()->QueuePriorityCommand(
			GetTypeId<LeaveMetalMineDriveWayCommand>(), &ct);

		// Update with those overMs to ensure consistency
		Update(overms);
	}

	DllExclusive void MineLoadingPackageToTransporterState::OnCraneReturned(float overms)
	{
		// Crane just returned to its inital spot, so end this state and switch
		// to waiting for next package.
		_owner->ChangeState(GetTypeId<MineWaitingForPackageState>());
		// Of course update it
		_owner->Update(overms);
	}

	DllExclusive MineReturningWithPackageState::MineReturningWithPackageState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void MineReturningWithPackageState::Begin()
	{
		// This state can be only changed from 'MineLoadingPackageToTransporterState', 
		// before package is loaded into transporter, so some things are sure :
		// - if current animation is "PickUpPackage", then package is intact, so just
		// reverse animation
		// - if current animation is "TransportPackage", then we need to reverse crane and
		// package animation to the above point
		if(_owner->GetAnimController().GetCurrentAnimations()[0]->
			GetName().compare("PickUpPackage") == 0)
		{
			_packageDropped = true;
		}
		else if(_owner->GetAnimController().GetCurrentAnimations()[0]->
			GetName().compare("TransportPackage") == 0)
		{
			_packageDropped = false;
		}
		else ; // Something went wrong
	}

	DllExclusive void MineReturningWithPackageState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void MineReturningWithPackageState::Update(float ms)
	{
		_owner->GetCommandQueue()->Update(ms);
		_owner->GetAnimController().Update(-ms); // -> well it may not work
	}

	DllExclusive MineWaitingForPackageState::MineWaitingForPackageState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void MineWaitingForPackageState::Begin()
	{	
		MetalMineObjectData* metalMineData = _owner->GetObjectSpecificData<MetalMineObjectData>();
		MetalMineTypeData* metalMineTypeData = _owner->GetKindSpecificData<MetalMineTypeData>();

		if(metalMineData->AvailableMetal <= 0)
		{
			// No metal remains -> change to depleted state later
			_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "Depleted");
		}
		else
		{
			_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "PreparingPackage");

			// Start package timer
			metalMineData->PackageTimer = metalMineTypeData->PackageWaitTime;
		}
	}

	DllExclusive void MineWaitingForPackageState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void MineWaitingForPackageState::Update(float ms)
	{
		UnitState::Update(ms);

		MetalMineObjectData* metalMineData = _owner->GetObjectSpecificData<MetalMineObjectData>();
		MetalMineTypeData* metalMineTypeData = _owner->GetKindSpecificData<MetalMineTypeData>();
		if(metalMineData->Package != 0)
		{
			// Mine is waiting for package to move to its package spot
			if(metalMineData->Package->GetPosition().positionEquals(
				metalMineTypeData->PackageFinalSpot, 1.f))
			{
				// Package is in its target spot, so mark it as ready and sitch states
				metalMineData->PackageReady = true;
				_owner->ChangeState(GetTypeId<MinePackageReadyState>());
			}
		}
		else if(metalMineData->AvailableMetal > 0)
		{
			metalMineData->PackageTimer -= ms;
			if(metalMineData->PackageTimer < 0.0f)
			{
				// Create package
				SceneObject* package = 
					GameWorld::GlobalWorld->CreateSceneObject(
					metalMineTypeData->PackageDefinitionHandle,
					NEUTRAL_PLAYERNUM);

				// Attach to mine ( it should be spawned there ) and move to proper spot
				_owner->AttachChild(package, true);
				package->SetPosition(metalMineTypeData->PackageSpawnSpot);

				metalMineData->Package = static_cast<PhysicalObject*>(package);
				int metalUsed = std::min(metalMineTypeData->PackageMetalAmount, metalMineData->AvailableMetal);
				metalMineData->AvailableMetal -= metalUsed;
				package->GetObjectSpecificData<PackageObjectData>()->StoredMetal = metalUsed;

				// Start moving package and update it with over time (equal to negative time on package timer)
				package->ChangeState(GetTypeId<PackageMovingOnTransmissionBeltState>());
				package->Update(-metalMineData->PackageTimer);
			}
		}
	}
}