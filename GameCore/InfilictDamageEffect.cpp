#include "InflictDamagaEffect.h"
#include "Weapon.h"
#include "Missle.h"
#include "Unit.h"

namespace FlagRTS
{
	InflictDamageEffect::InflictDamageEffect()
	{
		SetFinalType(GetTypeId<InflictDamageEffect>());
		SetFinalTypeName("Effect"); // Type name for all effects is Effect
	}

	InflictDamageEffect::InflictDamageEffect(XmlNode* xml) :
		IEffect(xml)
	{
		SetFinalType(GetTypeId<InflictDamageEffect>());
		SetFinalTypeName("Effect"); // Type name for all effects is Effect
	}

	void InflictDamageEffect::ApplyEffect(IGameObject* caster,
		IGameObject* source,
		IGameObject* target,
		const Vector3& targetPoint)
	{
		if( target == 0 || target->GetFinalType() != GetTypeId<Unit>()) // No object hit or some non-damagable -> no damage then
			return;

		_ASSERT( source->GetFinalType() == GetTypeId<Missle>() ||
			source->GetFinalType() == GetTypeId<Weapon>());

		// Check all conditions on target

		if( source->GetFinalType() == GetTypeId<Missle>() )
		{
			Missle* missle = static_cast<Missle*>(source);
			// Get weapon or ability that launched missle
			if( missle->GetSource()->GetFinalType() == GetTypeId<Weapon>() )
			{
				InflictWeaponDamage(static_cast<Weapon*>(missle->GetSource()),
					static_cast<Unit*>(target));
			}
		}
		else if( source->GetFinalType() == GetTypeId<Weapon>() )
		{
			InflictWeaponDamage(static_cast<Weapon*>(source),
				static_cast<Unit*>(target));
		}

	}

	void InflictDamageEffect::InflictWeaponDamage(Weapon* weapon, Unit* target)
	{
		int damage = weapon->ComputeWeaponDamageVsTarget(target);
		target->InflictDamage(damage, weapon);
	}
}