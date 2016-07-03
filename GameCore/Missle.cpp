#include "Missle.h"
#include "GameWorld.h"
#include "PathingSystem.h"
#include "IEffect.h"
#include "MissleHolder.h"
#include "Unit.h"

namespace FlagRTS
{
	Missle::Missle(MissleDefinition* def) :
		PhysicalObject(static_cast<PhysicalObjectDefinition*>(def)),
		_caster(0),
		_source(0),
		_sourceHolder(0),
		_target(0)
	{
		SetFinalType(GetTypeId<Missle>());
	}

	Missle::~Missle()
	{

	}

	void Missle::Spawn()
	{
		if(!_spawned)
		{
			PhysicalObject::Spawn();
			_stateMachine.ChangeState(SceneObjectStates::Idle);
			_spawned = true;
			_onSpawn(this);
		}
	}

	void Missle::Despawn()
	{
		if(_spawned)
		{
			_stateMachine.ChangeState(SceneObjectStates::Idle);
			PhysicalObject::Despawn();
			_spawned = false;
			_onDespawn(this);
		}
	}

	void Missle::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			PhysicalObject::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void Missle::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			PhysicalObject::UnloadResources(ogreMgr);
			_loaded = false;
		}
	}

	void Missle::Launch(PhysicalObject* target)
	{
		_target = target;
		_targetPosition = Vector3::ZERO;
		// Orient missle towards target
		// TODO rewrite getRotationTo to some more faster version
		Quaternion rotationToTarget = GetDirectionAbsolute().getRotationTo(
			_target->GetPositionAbsolute() - GetPositionAbsolute());
		Rotate(rotationToTarget, Ogre::Node::TS_WORLD);
		
		_stateMachine.ChangeState(SceneObjectStates::Move);
	}

	void Missle::Launch(PhysicalObject* target, const Vector3& offset)
	{
		_target = target;
		_targetPosition = offset;
		// Orient missle towards target
		// TODO rewrite getRotationTo to some more faster version
		Quaternion rotationToTarget = GetDirectionAbsolute().getRotationTo(
			_target->GetPositionAbsolute() - GetPositionAbsolute() + _targetPosition);
		Rotate(rotationToTarget, Ogre::Node::TS_WORLD);

		_stateMachine.ChangeState(SceneObjectStates::Move);
	}

	void Missle::Launch(const Vector3& target)
	{
		_target = 0;
		_targetPosition = target;
		Quaternion rotationToTarget = GetDirectionAbsolute().getRotationTo(
			_targetPosition - GetPositionAbsolute());
		Rotate(rotationToTarget, Ogre::Node::TS_WORLD);

		_stateMachine.ChangeState(SceneObjectStates::Move);
	}

	void Missle::HitObject(PhysicalObject* object, const Vector3& pos)
	{
		_stateMachine.ChangeState(SceneObjectStates::Dying);
		
		if( GetMissleDefinition()->GetOnImpactEffect() != 0 )
		{
			GetMissleDefinition()->GetOnImpactEffect()->ApplyEffect(
				_caster, _source, object, pos);
		}

		_onImpact.Fire(this, object, pos, false);
	}

	void Missle::HitGround(const Vector3& pos)
	{
		_stateMachine.ChangeState(SceneObjectStates::Dying);
		
		if( GetMissleDefinition()->GetOnImpactEffect() != 0 )
		{
			GetMissleDefinition()->GetOnImpactEffect()->ApplyEffect(
				_caster, _source, 0, pos);
		}

		_onImpact.Fire(this, 0, pos, true);
	}

	void Missle::Detonate(const Vector3& pos)
	{
		_stateMachine.ChangeState(SceneObjectStates::Dying);

		if( GetMissleDefinition()->GetOnImpactEffect() != 0 )
		{
			GetMissleDefinition()->GetOnImpactEffect()->ApplyEffect(
				_caster, _source, 0, pos);
		}

		_onImpact.Fire(this, 0, pos, false);
	}
}