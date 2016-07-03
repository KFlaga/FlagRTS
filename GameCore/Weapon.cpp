#include "Weapon.h"
#include "LaunchMissleEffect.h"
#include "Unit.h"

namespace FlagRTS
{

	Weapon::Weapon(WeaponDefinition* def) :
		_definition(def),
		_owner(0),
		_cooldown(0.f)
	{
		SetFinalType(GetTypeId<Weapon>());

		if( _definition->GetLaunchMissleEffect() != 0 )
		{
			_definition->GetLaunchMissleEffect()->AddUser();
		}
	}

	Weapon::~Weapon()
	{
		if( _definition->GetLaunchMissleEffect() != 0 )
		{
			_definition->GetLaunchMissleEffect()->RemoveUser();
		}
	}

	void Weapon::Update(float ms)
	{
		_cooldown -= ms;
	}

	void Weapon::AttackTarget()
	{
		_cooldown = _definition->GetStats().AttacksDelay;

		if( _definition->GetOnAttackEffect() != 0 )
		{
			_definition->GetOnAttackEffect()->ApplyEffect(
				_owner,
				this,
				_target.GetTargetUnit(),
				Vector3(0.f,0.f,0.f)); // Here should be contact point between weapon(caster) 
									   // and target offset from target center ( in caster orientation )
		}

		if( _definition->GetLaunchMissleEffect() != 0 )
		{
			_definition->GetLaunchMissleEffect()->ApplyEffect(
				_owner,
				this,
				_target.GetTargetUnit(),
				Vector3(0.f,0.f,0.f));
		}

		_onAttack.Fire(this, _owner, &_target);
	}

	float Weapon::ComputeWeaponEffeciencyVsTarget(Unit* target)
	{
		return ComputeWeaponDamageVsTarget(target) *  
			_definition->GetStats().AttackSpeed * 
			_definition->GetStats().AttacksCount;
	}

	int Weapon::ComputeWeaponDamageVsTarget(Unit* target)
	{
		return _definition->GetStats().Damage;
	}
}