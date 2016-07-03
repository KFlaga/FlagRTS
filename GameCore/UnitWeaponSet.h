
#pragma once

#include <Array.h>
#include "DataTypes.h"

namespace FlagRTS
{

	class Unit;
	class Weapon;
	class WeaponDefinition;
	class CommandTarget;

	// Set of weapons that unit can use
	// Except storing them also determines which weapons to use against target
	// Stores and switches active weapon -> attack state/command can read it 
	// and use it
	// If unit have multiple weapons for the target it may use only one
	// If unit should use more than one weapon it should have it 'turreted', so
	// basicaly weapon will fire independly of unit ( ofc attack command will try to focus
	// all weapons on same target )
	class UnitWeaponSet
	{
	private:
		Unit* _owner;
		Array<Weapon*> _weapons;
		Weapon* _currentWeapon;

	public:
		UnitWeaponSet();
		UnitWeaponSet(Unit* owner, const Array<std::pair<WeaponDefinition*, bool>>& weaponList);
		~UnitWeaponSet();

		std::pair<Weapon*, float> GetWeaponWithMaxRangeAgainstTarget(Unit* target);

		// Refreshes weapon cooldowns
		void Update(float ms);

		// Returns all unit's weapons
		const Array<Weapon*>& GetWeapons() const;

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