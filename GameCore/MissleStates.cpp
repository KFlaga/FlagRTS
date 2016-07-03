#include "MissleStates.h"
#include "Missle.h"
#include "AnimationController.h"
#include "MissleHolder.h"
#include "IMover.h"
#include <PathFindingUtils.h>
#include "Unit.h"
#include "CollisionArea.h"

namespace FlagRTS
{
	MissleIdleState::MissleIdleState(Missle* owner) :
		_owner(owner)
	{

	}

	void MissleIdleState::Update(float ms)
	{

	}

	const char* MissleIdleState::GetName()
	{
		return "Idle";
	}

	size_t MissleIdleState::GetType()
	{
		return SceneObjectStates::Idle;
	}

	MissleMoveState::MissleMoveState(Missle* owner) :
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
	}

	void MissleMoveState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void MissleMoveState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);

		_totalDistance += _owner->GetMover()->Move(_owner, _moveTime, ms);
		_moveTime += ms;

		CollisionArea missleCollision(
			PathFinding::CollisionFilter(_owner->GetPathingGroup(), _owner->GetPathingBlockedGroups()).Value,
			CollisionShapes::Box,
			_owner->GetPositionAbsolute(),
			_owner->GetHalfSize());
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
				_owner->HitObject(collsions[i], _owner->GetPositionAbsolute());
				return;
			}
		}

		if( _totalDistance >= _owner->GetMaxRange() )
		{
			_owner->Detonate(_owner->GetPositionAbsolute());
		}
	}

	const char* MissleMoveState::GetName()
	{
		return "Move";
	}

	size_t MissleMoveState::GetType()
	{
		return SceneObjectStates::Move;
	}

	MissleDyingState::MissleDyingState(Missle* owner) :
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
	}

	void MissleDyingState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void MissleDyingState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);
		if( _owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished() )
		{
			// Animation ended -> release missle
			_owner->GetSourceMissleHolder()->ReleaseMissle(_owner);
		}
	}

	const char* MissleDyingState::GetName()
	{
		return "Dying";
	}

	size_t MissleDyingState::GetType()
	{
		return SceneObjectStates::Dying;
	}
}