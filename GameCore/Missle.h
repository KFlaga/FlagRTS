#pragma once

#include "MissleDefinition.h"
#include "PhysicalObject.h"

namespace FlagRTS
{
	class IMover;
	class Weapon;
	class Unit;
	class MissleHolder;

	class Missle : public PhysicalObject
	{
	protected:
		Unit* _caster;
		IGameObject* _source; // Source may be weapon or ability
		MissleHolder* _sourceHolder;

		// Args: 1) this missle, 2) hit object or 0, 3) hit position, 4) true if hit/false if detonated before hitting
		Event<Missle*, PhysicalObject*, const Vector3&, bool> _onImpact;
		PhysicalObject* _target;
		Vector3 _targetPosition;


	public:
		Missle(MissleDefinition* def);
		~Missle();

		MissleDefinition* GetMissleDefinition() const { return static_cast<MissleDefinition*>(_definition); }
		
		void SetCaster(Unit* caster) { _caster = caster; }
		Unit* GetCaster() const { return _caster; }
		
		void SetSource(IGameObject* caster) { _source = caster; }
		IGameObject* GetSource() const { return _source; }

		IMover* GetMover() const { return GetMissleDefinition()->GetMover(); }

		void SetSourceMissleHolder(MissleHolder* storage) { _sourceHolder = storage; }
		MissleHolder* GetSourceMissleHolder() const { return _sourceHolder; }
		
		float GetMaxRange() const { return GetMissleDefinition()->GetMaxRange(); }

		void Spawn();
		void Despawn();
		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Launch(PhysicalObject* target);
		void Launch(PhysicalObject* target, const Vector3& offset);
		void Launch(const Vector3& target);

		void HitObject(PhysicalObject* object, const Vector3& pos);
		void HitGround(const Vector3& pos);
		void Detonate(const Vector3& pos);
	};
}