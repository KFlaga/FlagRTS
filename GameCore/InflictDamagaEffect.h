#pragma once

#include "IEffect.h"
#include "ObjectDefinition.h"

namespace FlagRTS
{
	class Weapon;

	// Inflict damage effect is separate effect, considering how common it may be
	// It basicaly inflicts damage on target unit
	// Unit may need to meet some requirements ( like being ground, which may be useful for aoe effects )
	// Damage is taken from Source -> it should be Weapon/Ability or Missle class object
	// If its missle then Weapon/Ability is taken from it
	class InflictDamageEffect : public IEffect
	{
		// Array<ICondition*> _applyConditions;

	public:
		InflictDamageEffect();
		InflictDamageEffect(XmlNode* xml);

		// Inflicts damage calculated by source weapon/ability to target
		// Caster must be Unit or 0 ( not used here actualy )
		// Source must be Missle/Weapon/Ability
		// Target must be Unit
		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint);

		// void AddCondition(ICondition* condition);
		// void RemoveCondition(ICondition* condition);

	private:
		void InflictWeaponDamage(Weapon* weapon, Unit* target);
	};
}