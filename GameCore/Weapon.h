#pragma once

#include "WeaponDefinition.h"
#include "CommandTarget.h"
#include "IGameObject.h"

namespace FlagRTS
{
	class Unit;
	class IEffect;

	class Weapon : public IGameObject
	{
	protected:
		WeaponDefinition* _definition;
		Event<Weapon*, Unit*, CommandTarget*> _onAttack;
		bool _isActive;
		float _cooldown;

		Unit* _owner;
		CommandTarget _target; // Current target of attack

	public:
		Weapon(WeaponDefinition* def);
		virtual ~Weapon();

		// Attack target with this weapon -> it should be called in proper moment of animation
		// ( when hit / fire etc. occurs ). By default sets cooldown, target and fires event
		virtual void AttackTarget();

		void SetTarget(CommandTarget& target) { _target = target; }
		const CommandTarget& GetTarget() const { return _target; }

		void SetOwner(Unit* owner) { _owner = owner; }
		Unit* GetOwner() const { return _owner; }

		// Returns true if weapon can be used ( that is is active and not on cooldown )
		bool IsAvailable() const
		{
			return _isActive && ( _cooldown <= 0.0f );
		}

		// Reduces weapon cooldown by default
		// Derives may i.e. update fired missle
		virtual void Update(float ms);

		// Return effeciency coefficient vs given target, considering weapon type,
		// damage, speed, target type, armor etc.
		float ComputeWeaponEffeciencyVsTarget(Unit* target);

		// Returns damage to be infilicted to unit on hit with given weapon
		// ( takes into accout weapon type and target armor )
		// ( for splash damage it should be scaled by proper coefficient )
		int ComputeWeaponDamageVsTarget(Unit* target);

		// Weapon handle used i.e. in abilities which switch weapons
		size_t GetWeaponHandle() const { return _definition->GetHandle().Object; }
		const string& GetActionName() const { return _definition->GetActionName(); }
		float GetAttackTime() const { return _definition->GetAttackTime(); }
		Degree GetAttackAngle() const { return _definition->GetAttackAngle(); }
		const WeaponStats& GetStats() const { return _definition->GetStats(); }
		WeaponStats& GetStats() { return _definition->GetStats(); }
		const string& GetIcon() const { return _definition->GetIcon(); }
		const string& GetGuiName() const { return _definition->GetGuiName(); }
		WeaponType GetWeaponType() const { return _definition->GetWeaponType(); }

		const string& GetName() const { return _definition->GetName(); }
	
		bool IsActive() const { return _isActive; }
		void SetIsActive(bool value) { _isActive = value; }

		float GetCooldown() const { return _cooldown; }

		int GetAvailableTargets() const { return _definition->GetAvailableTargets(); }
		
		Event<Weapon*, Unit*, CommandTarget*>& GetOnAttackEvent() { return _onAttack; }
	};
}