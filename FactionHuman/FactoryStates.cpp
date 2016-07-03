#pragma once

#include "FactoryStates.h"
#include "FactoryData.h"
#include "FactoryOrders.h"
#include <Unit.h>
#include <TrainingQueue.h>
#include <GameWorld.h>
#include <AnimationController.h>

namespace FlagRTS
{
	FactoryDoorOpeningState::FactoryDoorOpeningState(Unit* owner) :
		UnitTrainingState(owner)
	{ }

	void FactoryDoorOpeningState::Begin()
	{
		// Play open doors animation
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		objectData->DoorsClosed = false;
		objectData->DoorsOpened = false;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "OpenDoors", false);
	}

	void FactoryDoorOpeningState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void FactoryDoorOpeningState::Update(float ms)
	{
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		// Wait for doors to be fully opened and set open doors flag
		// If there's unit in factory change state to UnitMovingOut else to Idle ( should not happen actually )
		auto& anims = _owner->GetAnimController().GetCurrentAnimations();

		if(anims[0]->CheckIsFinished())
		{
			objectData->DoorsOpened = true;
			if(objectData->UnitInside != 0)
			{
				_owner->ChangeState(GetTypeId<FactoryUnitMovingOutState>());
			}
			else
			{
				_owner->ChangeState(SceneObjectStates::Idle);
			}

			return;
		}
		UnitTrainingState::Update(ms);
	}

	bool FactoryDoorOpeningState::TryFinishTrain(Unit* trainedUnit)
	{
		return false;
	}

	FactoryDoorClosingState::FactoryDoorClosingState(Unit* owner) :
		UnitTrainingState(owner)
	{

	}

	void FactoryDoorClosingState::Begin()
	{
		// Doors must be opened
		// Order doors to close themselves
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		objectData->DoorsClosed = false;
		objectData->DoorsOpened = false;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "CloseDoors", false);
	}

	void FactoryDoorClosingState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void FactoryDoorClosingState::Update(float ms)
	{
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		// Wait for doors to be fully opened and set open doors flag, change state to idle
		auto& anims = _owner->GetAnimController().GetCurrentAnimations();

		if(anims[0]->CheckIsFinished())
		{
			objectData->DoorsClosed = true;
			// If factory is training another unit - change state to training
			if(_owner->GetTrainingQueue()->IsTraining())
			{
				_owner->ChangeState(SceneObjectStates::Train);
			}
			else
			{
				_owner->ChangeState(SceneObjectStates::Idle);
			}

			return;
		}
		UnitTrainingState::Update(ms);
	}

	bool FactoryDoorClosingState::TryFinishTrain(Unit* trainedUnit)
	{
		return false;
	}

	FactoryTrainingState::FactoryTrainingState(Unit* owner) :
		UnitTrainingState(owner)
	{

	}

	void FactoryTrainingState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "ClosedIdle");
	}

	void FactoryTrainingState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void FactoryTrainingState::Update(float ms)
	{

		UnitTrainingState::Update(ms);
	}

	bool FactoryTrainingState::TryFinishTrain(Unit* trainedUnit)
	{
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		FactoryTypeData* typeData = _owner->GetKindSpecificData<FactoryTypeData>();
		if( objectData->UnitInside == 0 )
		{
			// Spawn unit on InsideSpot, order open doors
			GameWorld::GlobalWorld->SpawnSceneObject(trainedUnit, 
				SpawnInfo(_owner->GetOrientation(), _owner->GetPosition() + typeData->InsideSpot));

			trainedUnit->SetIsInteractable(false);
			trainedUnit->SetIsHoverable(false);
			trainedUnit->SetIsSelectable(false);
			// trainedUnit->SetIsImmortal(true)/SetIsAITransparent(true); -> till doors are not opened, unit cannot be killed
			// Add event -> factory died -> reset those flags

			objectData->UnitInside = trainedUnit;

			CommandTarget ct;
			_owner->ExecuteCommand(GetTypeId<FactoryOpenDoorsCommand>(), &ct);
			return true;
		}
		else
			return false;
	}

	FactoryUnitMovingOutState::FactoryUnitMovingOutState(Unit* owner) :
		UnitTrainingState(owner)
	{
		
	}

	void FactoryUnitMovingOutState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "OpenedIdle");

		// Doors opened - move unit to DoorSpot
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		FactoryTypeData* typeData = _owner->GetKindSpecificData<FactoryTypeData>();
		if( objectData->UnitInside == 0 )
		{
			// Unit was killed before it went out -> proceed to closing door immediately
			CommandTarget ct;
			_owner->ExecuteCommand(GetTypeId<FactoryCloseDoorsCommand>(), &ct);
		}
		else
		{
			// Just order it to move -> it should it on its own
			// For now ( before pathing gets a little better ) move it in updates

			// Compute some variables used for moving / checking if unit moved out :
			// left/right ( correct names assuming factory is not rotated ) ends of doors ( treated as line )
			_doorsLeft = _owner->GetOrientation() * (
				typeData->DoorCenter - Vector3(typeData->DoorsHalfLength, 0.f, 0.f)) + _owner->GetPosition();
			_doorsRight= _owner->GetOrientation() * (
				typeData->DoorCenter + Vector3(typeData->DoorsHalfLength, 0.f, 0.f)) + _owner->GetPosition();
			// Direction (scaled) from unit spawn position to exit
			_moveOutDir = _owner->GetOrientation() * (typeData->DoorCenter - typeData->InsideSpot);
			_moveOutDir.normalise();
			_moveOutDir *= 1000.f;
			// Constants used for detrmining if point is in same side of doors as factory center 
			//  d=(x−x1)(y2−y1)−(y−y1)(x2−x1) { (x1,y1),(x2,y2) are ends of line } : for one point d < 0, for other d > 0
			//  otherwise they are on same side of line
			_y2y1 = _doorsRight.z - _doorsLeft.z;
			_x2x1 = _doorsRight.x - _doorsLeft.x;
			_centerD = (_owner->GetPosition().x - _doorsLeft.x) * _y2y1 - (_owner->GetPosition().z - _doorsLeft.z)*_x2x1;
		}

	}

	void FactoryUnitMovingOutState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void FactoryUnitMovingOutState::Update(float ms)
	{
		// On door spot close doors + order unit to move to RallyPoint
		FactoryObjectData* objectData = _owner->GetObjectSpecificData<FactoryObjectData>();
		FactoryTypeData* typeData = _owner->GetKindSpecificData<FactoryTypeData>();
		if( objectData->UnitInside != 0 ) 
		{
			// Distance from final spot -> unit drove out if all of its OOBB corners are outside, that is
			// they are on side of doors other than factory center
			const Vector3& pos = objectData->UnitInside->GetPosition();
			const Vector3& factoryPos = _owner->GetPosition();
			auto unitBBox = objectData->UnitInside->GetLocalOrientedBoundingBox();

			// Now check if 4 corners lie outsie of factory:
			int i = 0;
			for(; i < 4; ++i)
			{
				Vector2 corner = unitBBox[i] + Vector2(pos.x, pos.z);
				float cornerD = (corner.x-_doorsLeft.x) * _y2y1 - (corner.y-_doorsLeft.z)*_x2x1;
				if( cornerD * _centerD > 0 ) // Corner is inside
					break;
			}
			if( i == 4 )
			{
				// All corners outside -> can close the doors and move unit to rally point
				if( _owner->GetRallyPoint().x > 0 )
				{
					CommandTarget ct;
					ct.SetPointTarget(_owner->GetRallyPoint());
					objectData->UnitInside->ExecuteCommand(OrderTypes::Move, &ct);
				}
				// Move it after doors are opened ( so to this state -> begin )
				objectData->UnitInside->SetIsInteractable(true);
				objectData->UnitInside->SetIsHoverable(true);
				objectData->UnitInside->SetIsSelectable(true);
				objectData->UnitInside = 0;

				CommandTarget ct;
				_owner->ExecuteCommand(GetTypeId<FactoryCloseDoorsCommand>(), &ct);
			}
			else
			{
				// Unit is still outside factory -> move unit in outside direction
				objectData->UnitInside->MoveUnit(factoryPos + typeData->DoorCenter + _moveOutDir , ms);
			}
		}
		else
		{
			// Unit was killed - close doors
			CommandTarget ct;
			_owner->ExecuteCommand(GetTypeId<FactoryCloseDoorsCommand>(), &ct);
		}

		UnitTrainingState::Update(ms);
	}

	bool FactoryUnitMovingOutState::TryFinishTrain(Unit* trainedUnit)
	{
		return false;
	}
}