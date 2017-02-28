#include "MissleStates.h"
#include "Missle.h"
#include "AnimationController.h"
#include "MissleHolder.h"
#include "IMover.h"
#include <PathFindingUtils.h>
#include "Unit.h"
#include "CollisionArea.h"
#include "CollisionComponent.h"

namespace FlagRTS
{
	MissleIdleState::MissleIdleState(Missle* owner) :
		SceneObjectState(SceneObjectStates::Idle, "Idle"),
		_owner(owner)
	{

	}

	void MissleIdleState::Update(float ms)
	{

	}

	MissleMoveState::MissleMoveState(Missle* owner) :
		SceneObjectState(SceneObjectStates::Move, "Move"),
		_owner(owner)
	{

	}

	void MissleMoveState::Begin()
	{
		auto anim = _owner->GetAnimController().FindAnimation(SceneObjectStates::Move, "Default");
		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(SceneObjectStates::Move, "Default", true);
		}
		_moveTime = 0.f;
		_totalDistance = 0.f;
		_status = StateStatus::RunningCritical;
	}

	void MissleMoveState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}
	
	void MissleMoveState::Interrupt()
	{
		_ASSERT(false); // Cannot be interrupted
	}
	
	void MissleMoveState::Resume()
	{
		_ASSERT(false); // Cannot be interrupted
	}

	void MissleMoveState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);

		_totalDistance += _owner->GetMover()->Move(_owner, _moveTime, ms);
		_moveTime += ms;

		_ASSERT(_owner->FindComponent<CollisionComponent>() != 0);
		CollisionComponent* collision = _owner->FindComponent<CollisionComponent>();

		CollisionArea missleCollision(
			collision->GetCollisionFilter().Value,
			CollisionShapes::Box,
			collision->GetCollisionShape().GetCenter(),
			collision->GetCollisionShape().GetHalfExtends());

		missleCollision.SetOwner(_owner->GetCaster());
		missleCollision.FindCollisions();
		auto collsions = missleCollision.GetUnitsHit();
		for(unsigned int i = 0; i < collsions.size(); ++i)
		{
			// We got actual hit, get first one
			// TODO consider situations :
			// - frame rate for this frame is REALLY bad and missle jumped through an object
			// - 2 objects are toching themsleves and missle went just between them
			if( collsions[i]->IsAlive() ) // Ignore dead units
			{
				_status = StateStatus::Finished;
				_owner->HitObject(collsions[i], _owner->GetPositionAbsolute());
				return;
			}
		}

		if( _totalDistance >= _owner->GetMaxRange() )
		{
			_status = StateStatus::Finished;
			_owner->Detonate(_owner->GetPositionAbsolute());
		}
	}

	MissleDyingState::MissleDyingState(Missle* owner) :
		SceneObjectState(SceneObjectStates::Dying, "Dying"),
		_owner(owner)
	{

	}

	void MissleDyingState::Begin()
	{
		auto anim = _owner->GetAnimController().FindAnimation(SceneObjectStates::Dying, "Default");
		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(SceneObjectStates::Dying, "Default", false);
		}
		else
		{
			// No animation -> release missle
			_owner->GetSourceMissleHolder()->ReleaseMissle(_owner);
		}

		_status = StateStatus::RunningCritical;
	}

	void MissleDyingState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}
	
	void MissleDyingState::Interrupt()
	{
		_ASSERT(false); // Cannot be interrupted
	}
	
	void MissleDyingState::Resume()
	{
		_ASSERT(false); // Cannot be interrupted
	}

	void MissleDyingState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);
		if( _owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished() )
		{
			_status = StateStatus::Finished;
			// Animation ended -> release missle
			_owner->GetSourceMissleHolder()->ReleaseMissle(_owner);
		}
	}
}