#include "UnitStates.h"
#include "Unit.h"
#include "AnimationController.h"
#include "CommandQueue.h"
#include "Weapon.h"
#define PROFILE_OFF
#include <Profiler.h>

#include "GameWorld.h"
#include "ISceneObjectSpawner.h"
#include "PathingSystem.h"
#include "Map.h"

#include "ConstructionSiteData.h"
#include "Resources.h"
#include "TrainingComponent.h"

namespace FlagRTS
{
	UnitState::UnitState(Unit* owner, size_t type, const char* name) :
		SceneObjectState(type, name),
		_owner(owner)
	{

	}

	void UnitState::Update(float ms)
	{
		_owner->GetCommandQueue()->Update(ms);
		_owner->GetAnimController().Update(ms);
	}

	UnitIdleState::UnitIdleState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Idle, "Idle"),
		_defaultActionLength(1e6f),
		_boredActionLength(1e7f),
		_boredTimer(0.f)
	{
	}

	void UnitIdleState::Begin()
	{
		_status = StateStatus::RunningNoncritical;

		_defaultActionLength = _owner->GetAnimController().
			FindAnimation(SceneObjectStates::Idle, "Default")->GetLength() * 3.f;

		auto boredAnim = _owner->GetAnimController().
			FindAnimation(SceneObjectStates::Idle, "Bored");
		if( boredAnim != 0 )
		{
			_boredActionLength = boredAnim->GetLength();
		}
		else
			_boredActionLength = -1.f;

		_boredTimer = 0.f;
		// Start idle animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "Default");
		// Allow unit movement
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, false);
	}

	void UnitIdleState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}

	void UnitIdleState::Interrupt()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}

	void UnitIdleState::Resume()
	{
		Begin();
	}

	void UnitIdleState::Update(float ms)
	{
		if( GameWorld::GlobalWorld->GetPathingSystem()->ShouldUnitMove(_owner) )
		{
			_owner->ChangeState(SceneObjectStates::Move);
			_owner->Update(ms);
		}
		else
		{
			if( _boredActionLength > 0.f )
			{
				_boredTimer += ms;
				if(_boredTimer > _defaultActionLength + 3*_boredActionLength)
				{
					_owner->GetAnimController().ChangeAnimation(
						SceneObjectStates::Idle, "Default");
					_boredTimer = 0.f;
				}
				else if(_boredTimer > _defaultActionLength &&
					_boredTimer < _boredActionLength)
				{
					_owner->GetAnimController().ChangeAnimation(
						SceneObjectStates::Idle, "Bored");
					_boredTimer += 2*_boredActionLength;
				}
			}
		}
		UnitState::Update(ms);
	}

	UnitMoveState::UnitMoveState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Move, "Move")
	{

	}

	void UnitMoveState::Begin()
	{
		_status = StateStatus::RunningNoncritical;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Move, "Default");
		// Allow unit movement
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, false);
	}

	void UnitMoveState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitMoveState::Resume()
	{
		Begin();
	}

	void UnitMoveState::Update(float ms)
	{
		if( GameWorld::GlobalWorld->GetPathingSystem()->ShouldUnitMove(_owner) )
		{
			const Vector2& move2d = GameWorld::GlobalWorld->GetPathingSystem()->GetUnitNextMove(_owner);
			const Vector3& position = _owner->GetPositionAbsolute();
			float y = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(position.x + move2d.x,  position.z + move2d.y);
			Vector3 moveTo(position.x + move2d.x, y, position.z + move2d.y);

			_owner->MoveUnit(moveTo, ms);
		}
		// else if() -> if not reach goal but cannot move, after few secs abort move
		else
		{
			_status = StateStatus::Finished;
		}
		UnitState::Update(ms);
	}

	UnitForceMoveState::UnitForceMoveState(Unit* owner) :
		UnitState(owner, SceneObjectStates::ForceMove, "ForceMove")
	{

	}

	void UnitForceMoveState::Begin()
	{
		_status = StateStatus::RunningCritical;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Move, "Default");
		// Disallow unit movement
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, true);
	}

	void UnitForceMoveState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitForceMoveState::Update(float ms)
	{
		_owner->MoveUnit(_target, ms);

		if(_owner->GetMoveStrategy()->GetRemainingSquaredDistance() <= 1e-3f)
		{
			// Unit moved to destination, so change state back
			_status = StateStatus::Finished;
		}
		UnitState::Update(ms);
	}

	UnitAttackState::UnitAttackState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Attack, "Attack")
	{

	}

	void UnitAttackState::Begin()
	{
		_status = StateStatus::RunningNoncritical;

		// Check current weapon != 0. It should be set by command/AI state
		Weapon* weapon = _owner->GetWeapons()->GetCurrentWeapon();
		_ASSERT(weapon != 0);
		_ASSERT(weapon->IsActive());
		if(weapon == 0)
		{
			_owner->ChangeState(SceneObjectStates::Idle);
		}

		// Check if weapon is available ( if not we need to wait for cooldown )
		if(weapon->IsAvailable())
		{
			// Weapon is available -> Start weapon animation
			_owner->GetAnimController().ChangeAnimation(
				SceneObjectStates::Attack, weapon->GetActionName(), false);
			_playingAnim = true;
		}
		else
			_playingAnim = false;
		_weaponUsed = false;

		// Make unit hold position during attack
		GameWorld::GlobalWorld->GetPathingSystem()->SetUnitHoldPosition(_owner, true);
	}

	void UnitAttackState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitAttackState::Update(float ms)
	{
		Weapon* weapon = _owner->GetWeapons()->GetCurrentWeapon();
		_ASSERT(weapon != 0);
		if(weapon == 0)
		{
			_status = StateStatus::Finished;
			return;
		}

		if( _playingAnim )
		{
			// Check if animation reached hit progress
			float hitTime = weapon->GetAttackTime();
			auto& anims = _owner->GetAnimController().GetCurrentAnimations();
			// Find weapon animation ( ot should be only animation )
			//for(unsigned int i = 0; i < anims.size(); ++i)
			//{
			//	if( anims[i]->GetAction().compare(weapon->GetActionName()) == 0 )
			//	{
			if( anims[0]->GetProgress() >= hitTime && _weaponUsed == false )
			{
				float overMs = ms + (anims[0]->GetProgress() - hitTime)*anims[0]->GetLength();
				// Hit occured overMs time ago, so:
				// 1) use weapon
				weapon->AttackTarget();
				// 2) Update weapon
				weapon->Update(overMs);
				_weaponUsed = true;
			}
			else if( anims[0]->CheckIsFinished() )
			{
				// If anim is finished change animation to cooldown
				_playingAnim = false;
				_weaponUsed = false;
				auto anim = _owner->GetAnimController().FindAnimation(SceneObjectStates::Attack, "Cooldown");
				if(anim != 0)
				{
					_owner->GetAnimController().ChangeAnimation(
						SceneObjectStates::Attack, "Cooldown", false);
				}
				else
				{
					_owner->GetAnimController().ChangeAnimation(
						SceneObjectStates::Idle, "Default", false);
				}
			}
			//		break;
			//	}
			//}
		}
		else
		{
			// Check if weapon is available ( if not we need to wait for cooldown )
			if(weapon->IsAvailable())
			{
				// Weapon is available -> play its animation again
				_owner->GetAnimController().ChangeAnimation(
					SceneObjectStates::Attack, weapon->GetActionName(), false);
				_playingAnim = true;
			}
		}
		UnitState::Update(ms);
	}

	UnitDyingState::UnitDyingState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Dying, "Dying"),
		_finished(false)
	{
	}

	void UnitDyingState::Begin()
	{
		_status = StateStatus::RunningCritical;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Dying, "Default", false);

		_finished = false;
	}

	void UnitDyingState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitDyingState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);

		if( _owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished() && _finished == false )
		{
			_status = StateStatus::Finished;
			_finished = true;
			//GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(_owner, true);
			GameInterfaces::GetSceneObjectSpawner()->DestroySceneObject(_owner, true);
		}
	}

	BuildingUnderConstructionState::BuildingUnderConstructionState(Unit* owner) :
		UnitState(owner, SceneObjectStates::UnderConstruction, "UnderConstruction")
	{

	}

	void BuildingUnderConstructionState::Begin()
	{
		_status = StateStatus::RunningNoncritical;
		_owner->SetIsUnderConstruction(true);
	}

	void BuildingUnderConstructionState::End()
	{
		_status = StateStatus::Ready;
		_owner->SetIsUnderConstruction(false);
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void BuildingUnderConstructionState::Update(float ms)
	{
		// Currently do nothing
	}

	ConstructionSiteBuildingState::ConstructionSiteBuildingState(Unit* owner) :
		UnitState(owner, GetTypeId<ConstructionSiteBuildingState>(), "Constructing"),
		_onBuildingDestroyed(this)
	{

	}

	void ConstructionSiteBuildingState::Begin()
	{
		_status = StateStatus::RunningNoncritical;

		// Site just spawned, now we should spawn the building
		ConstructionSiteObjectData* siteData = _owner->GetObjectSpecificData<ConstructionSiteObjectData>();
		ConstructionSiteTypeData* siteTypeData = _owner->GetKindSpecificData<ConstructionSiteTypeData>();

		siteData->Building->SetIsUnderConstruction(true);
		siteData->Building->SetConstructionProgress(0.f);

		_buildingHeight = siteData->Building->GetEntity()->getBoundingBox().getSize().y;
		// Spawn it fully under ground, it will emerge over time
		GameInterfaces::GetSceneObjectSpawner()->SpawnSceneObject(siteData->Building,
			SpawnInfo(_owner->GetOrientationAbsolute(), 
			_owner->GetPositionAbsolute() - Vector3(0.f, _buildingHeight-1.f, 0.f),  false));
		siteData->Building->ChangeState(SceneObjectStates::UnderConstruction);

		// Look for all resources of bulidng and if some is to take at instant, do it
		auto& cost = siteData->Building->GetCurrentStats()->Cost;
		for(unsigned int i = 0; i < cost.size(); ++i)
		{
			auto resources = GameWorld::GlobalWorld->GetResources();
			resources->TakeResourceFromPlayer(
				_owner->GetOwner(), resources->GetResourceNumber(cost[i].ResName), cost[i].ResAmount);
		}

		_constructionTime = 0;
		_tickTimer = siteTypeData->TickLength; // Update build progress every 500ms
		_tickBuildFraction = siteTypeData->TickLength / siteData->Building->GetCurrentStats()->BuildTime;

		siteData->Building->UnitDied() += &_onBuildingDestroyed;
	}

	void ConstructionSiteBuildingState::End()
	{
		_status = StateStatus::Ready;
		// End animation ??
		ConstructionSiteObjectData* siteData = _owner->GetObjectSpecificData<ConstructionSiteObjectData>();
		siteData->Building->UnitDied() -= &_onBuildingDestroyed;
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
	}

	void UnitForceMoveState::Resume()
	{
		_status = StateStatus::RunningNoncritical;
	}

	void ConstructionSiteBuildingState::Update(float ms)
	{
		// Update timer and check for tick. On tick:
		_tickTimer -= ms;
		while( _tickTimer < 0.f )
		{
			ConstructionSiteTypeData* siteTypeData = _owner->GetKindSpecificData<ConstructionSiteTypeData>();
			ConstructionSiteObjectData* siteData = _owner->GetObjectSpecificData<ConstructionSiteObjectData>();

			// Rise building proportinaly to time passed ( delta_height = total_height * tick / total_time )
			_constructionTime += (int)siteTypeData->TickLength;
			siteData->Building->MoveQuiet(Vector3(0.f, _buildingHeight * _tickBuildFraction, 0.f));
			siteData->Building->SetConstructionProgress( 
				siteData->Building->GetConstructionProgress() + _tickBuildFraction);
			siteData->Building->AddHitPoints((int)std::ceil(siteData->Building->GetMaxHitPoints() * _tickBuildFraction));
			// Check if construction is finished
			if(siteData->Building->GetConstructionProgress() >= 1.f)
			{
				_status = StateStatus::Finished;
				// Building finished : fire event
				siteData->Building->SetConstructionProgress(1.f);
				siteData->Building->SetHitPoints(std::min(
					siteData->Building->GetHitPoints(), siteData->Building->GetMaxHitPoints()));

				siteData->Building->ConstructionFinished().Fire(_owner, siteData->Building, false);
				siteData->Building->ConstructionFinished().Clear();
				return;
			}

			_tickTimer += siteTypeData->TickLength;
		}
		UnitState::Update(ms);
	}

	void ConstructionSiteBuildingState::OnBuildingDestroyed(Unit* building)
	{
		ConstructionSiteObjectData* siteData = 
			_owner->GetObjectSpecificData<ConstructionSiteObjectData>();
		siteData->Building->ConstructionFinished().Fire(_owner, siteData->Building, true);
		siteData->Building->ConstructionFinished().Clear();
	}

	UnitTrainingState::UnitTrainingState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Train, "Train")
	{

	}

	void UnitTrainingState::Begin()
	{
		_status = StateStatus::RunningNoncritical;
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Train, "Default");
	}

	void UnitTrainingState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitTrainingState::Update(float ms)
	{
		UnitState::Update(ms);
	}

	bool UnitTrainingState::TryFinishTrain(Unit* trainedUnit)
	{
		return true;
	}

	UnitOpeningState::UnitOpeningState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Open, "Open")
	{

	}

	void UnitOpeningState::Begin()
	{
		_status = StateStatus::RunningNoncritical;
	}

	void UnitOpeningState::End()
	{
		_status = StateStatus::Ready;
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitOpeningState::Update(float ms)
	{

	}

	UnitClosingState::UnitClosingState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Close, "Close")
	{

	}

	void UnitClosingState::Begin()
	{
		_status = StateStatus::RunningNoncritical;
	}

	void UnitClosingState::End()
	{
		_status = StateStatus::Ready;
	}

	void UnitForceMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
	}

	void UnitForceMoveState::Resume()
	{
		Begin();
	}

	void UnitClosingState::Update(float ms)
	{

	}
}