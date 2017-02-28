#pragma once

#include "IObjectComponent.h"
#include "ObjectDefinition.h"
#include <Array.h>

namespace FlagRTS
{
	class Unit;
	class Weapon;
	class WeaponDefinition;
	class CommandTarget;

	class WeaponComponentDefinition : public ObjectDefinition
	{
		DISALLOW_COPY(WeaponComponentDefinition);
	protected:
		Array<WeaponDefinition*> _weaponDefinitions;
		Array<bool> _activeWeapons; // List of weapons active by default

	public:
		WeaponComponentDefinition(XmlNode* componentNode);
		~WeaponComponentDefinition();

		Array<WeaponDefinition*>& GetWeaponDefinitions() { return _weaponDefinitions; }
		void SetWeaponDefinitions(const Array<WeaponDefinition*>& defs) { _weaponDefinitions = defs; }
		
		Array<bool>& GetActiveWeapon() { return _activeWeapons; }
		void GetActiveWeapon(const Array<bool>& aw) { _activeWeapons = aw; }
	};

	class WeaponComponent : public IObjectComponent
	{
	protected:
		Array<Weapon*> _weapons;
		Weapon* _currentWeapon;

	public:
		WeaponComponent(ObjectDefinition* definition, IGameObject* unit);
		~WeaponComponent();
		
		// Refreshes weapon cooldowns
		void Update(float ms);
		
		std::pair<Weapon*, float> GetWeaponWithMaxRangeAgainstTarget(Unit* target);
		
		// Returns weapon with given name ( == def name ) or 0 if not found
		Weapon* FindByName(const string& name);

		// Returns pair of :
		// - best weapon that can be currently used for given target or 0
		// if none can be used ( best = highest dps + within range )
		// - flag that indicates if target is within range of this weapon
		// ( if true is within range )
		// if target is beyond reach, unit should move closer before use
		std::pair<Weapon*, bool> GetWeaponForTarget(CommandTarget* target);
		// Returns weapon unit is attacking with now or 0 if
		// unit is not attacking
		Weapon* GetCurrentWeapon() const { return _currentWeapon; }
		// Sets weapon as currently used ( which means unit started to attack )
		void SetCurrentWeapon(Weapon* weapon) { _currentWeapon = weapon; }
		// Reset currently used weapons ( which means unit ended attack )
		void ReleaseWeapon() { _currentWeapon = 0; }
	};
}
