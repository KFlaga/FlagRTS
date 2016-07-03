#include "PowerPlantStates.h"
#include <Unit.h>
#include "PowerPlantData.h"
#include "EcoResources.h"
#include <GameWorld.h>
#include <AnimationController.h>


namespace FlagRTS
{
	DllExclusive PowerPlantWorkingState::PowerPlantWorkingState(Unit* owner) :
		UnitState(owner)
	{ }

	DllExclusive void PowerPlantWorkingState::Begin()
	{	
		PowerPlantTypeData* metalMineTypeData = _owner->GetKindSpecificData<PowerPlantTypeData>();

		_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Idle, "Default");
		_tickTimer = metalMineTypeData->TickLength;
	}

	DllExclusive void PowerPlantWorkingState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void PowerPlantWorkingState::Update(float ms)
	{
		UnitState::Update(ms);
		
		PowerPlantTypeData* metalMineTypeData = _owner->GetKindSpecificData<PowerPlantTypeData>();
		PowerPlantObjectData* metalMineObjectData = _owner->GetObjectSpecificData<PowerPlantObjectData>();
		_tickTimer -= ms;
		if(_tickTimer <= 0.f)
		{
			// Add resources -> later may add event on tick, so some floating text may appear
			GameWorld::GlobalWorld->GetResources()->AddResourceToPlayer
				(_owner->GetOwner(), EcoResources::Energy, metalMineObjectData->EnergyPerTick);

			_tickTimer += metalMineTypeData->TickLength;
		}
	}
}