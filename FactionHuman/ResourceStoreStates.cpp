#pragma once

#include "ResourceStoreStates.h"
#include "TransporterData.h"
#include "ResourceStoreData.h"
#include "TransporterOrders.h"
#include <Unit.h>
#include <AnimationController.h>

namespace FlagRTS
{
	StoreDoorOpeningState::StoreDoorOpeningState(Unit* owner) :
		UnitState(owner)
	{ }

	void StoreDoorOpeningState::Begin()
	{
		// Play open doors animation
		ResourceStoreObjectData* storeData = _owner->GetObjectSpecificData<ResourceStoreObjectData>();
		storeData->DoorsClosed = false;
		storeData->DoorsOpened = false;

		// Add animation to current one
		_owner->GetAnimController().AddAnimation(
			SceneObjectStates::Internal, "OpenDoors", false);
	}

	void StoreDoorOpeningState::End()
	{
		_owner->GetAnimController().EndAnimation(
			SceneObjectStates::Internal, "OpenDoors");
	}

	void StoreDoorOpeningState::Update(float ms)
	{
		ResourceStoreObjectData* storeData = _owner->GetObjectSpecificData<ResourceStoreObjectData>();
		// Wait for doors to be fully opened and set open doors flag, change state to idle
		auto& anims = _owner->GetAnimController().GetCurrentAnimations();
		for(unsigned int i = 0; i < anims.size(); ++i)
		{
			if(anims[i]->GetName().compare("OpenDoors") == 0)
			{
				if(anims[i]->CheckIsFinished())
				{
					storeData->DoorsOpened = true;
					_owner->ChangeState(SceneObjectStates::Idle);
					return;
				}
			}
		}
		UnitState::Update(ms);
	}

	StoreDoorClosingState::StoreDoorClosingState(Unit* owner) :
		UnitState(owner)
	{

	}

	void StoreDoorClosingState::Begin()
	{
		// Doors must be opened
		// Order doors to close themselves
		ResourceStoreObjectData* storeData = _owner->GetObjectSpecificData<ResourceStoreObjectData>();
		storeData->DoorsClosed = false;
		storeData->DoorsOpened = false;

		// Add animation to current one
		_owner->GetAnimController().AddAnimation(
			SceneObjectStates::Internal, "CloseDoors", false);
	}

	void StoreDoorClosingState::End()
	{
		_owner->GetAnimController().EndAnimation(
			SceneObjectStates::Internal, "CloseDoors");
	}

	void StoreDoorClosingState::Update(float ms)
	{
		ResourceStoreObjectData* storeData = _owner->GetObjectSpecificData<ResourceStoreObjectData>();
		// Wait for doors to be fully opened and set open doors flag, change state to idle
		auto& anims = _owner->GetAnimController().GetCurrentAnimations();
		for(unsigned int i = 0; i < anims.size(); ++i)
		{
			if(anims[i]->GetName().compare("CloseDoors") == 0)
			{
				if(anims[i]->CheckIsFinished())
				{
					storeData->DoorsClosed = true;
					_owner->ChangeState(SceneObjectStates::Idle);
					return;
				}
			}
		}
		UnitState::Update(ms);
	}

	StoreIdleState::StoreIdleState(Unit* owner) :
		UnitState(owner),
		_processingTimer(0.f),
		_processing(false)
	{

	}

	void StoreIdleState::Begin()
	{
		ResourceStoreObjectData* storeData = _owner->GetObjectSpecificData<ResourceStoreObjectData>();
		ResourceStoreTypeData* storeTypeData = _owner->GetKindSpecificData<ResourceStoreTypeData>();
		if(storeData->NewResourcesStored == true)
		{
			storeData->NewResourcesStored = false;
			if(_processing)
			{
				_processingTimer += storeTypeData->PackageProcessingTime;
			}
			else
			{
				_processingTimer = storeTypeData->PackageProcessingTime;
				_owner->GetAnimController().ChangeAnimation(
					SceneObjectStates::Idle, "Processing");
			}
			_processing = true;
		}
		else if(_processing == false)
		{
			_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "Default");
		}
	}

	void StoreIdleState::End()
	{
		
	}

	void StoreIdleState::Update(float ms)
	{
		_processingTimer -= ms;
		if(_processingTimer <= 0.f && _processing == true)
		{
			// Finished processing package
			_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "Default");
			_processing = false;
		}
		UnitState::Update(ms);
	}
}