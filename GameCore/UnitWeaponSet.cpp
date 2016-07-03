#include "UnitWeaponSet.h"
#include "Weapon.h"
#include "Unit.h"

namespace FlagRTS
{
	UnitWeaponSet::UnitWeaponSet()
	{

	}

	UnitWeaponSet::UnitWeaponSet(Unit* owner, const Array<std::pair<WeaponDefinition*, bool>>& weaponList) :
		_owner(owner)
	{
		_weapons.reserve(weaponList.size());
		// Create weapons
		for(auto wIt = weaponList.cbegin(), end = weaponList.cend();
			wIt != end; ++wIt)
		{
			Weapon* weapon =  xNew1(Weapon,wIt->first);
			weapon->SetIsActive(wIt->second);
			weapon->SetOwner(owner);
			_weapons.push_back(weapon);
		}
	}

	UnitWeaponSet::~UnitWeaponSet()
	{
		for(unsigned int i = 0; i < _weapons.size(); ++i)
		{
			xDelete(_weapons[i]);
		}
	}

	void UnitWeaponSet::Update(float ms)
	{
		for(auto weaponIt = _weapons.begin(), end = _weapons.end();
			weaponIt != end; ++weaponIt)
		{
			(*weaponIt)->Update(ms);
		}
	}

	std::pair<Weapon*, bool> UnitWeaponSet::GetWeaponForTarget(CommandTarget* target)
	{
		Weapon* bestWeapon = 0;
		bool inRange = false;

		// Find all weapons valid for given target, then for each find if it is in range
		// and lastly if it's better than previously found one
		for(auto weaponIt = _weapons.begin(), end = _weapons.end();
			weaponIt != end; ++weaponIt)
		{
			int validClasses = (*weaponIt)->GetAvailableTargets();
			// retrieve unit class ( if its ground/air etc ) and other flags
			// and compare with weapon's ones
			UnitClass targetClass = target->GetTargetUnit()->GetUnitClass();
			bool validWeapon = (validClasses & targetClass) != 0;
			if(validWeapon)
			{
				if(bestWeapon == 0)
					bestWeapon = *weaponIt;
				// Check if it is in range
				int range = (*weaponIt)->GetStats().Range;
				if(range >= target->GetTargetUnit()->GetDistanceToOtherObject(_owner))
				{
					inRange = true;
					// Get best range weapon for now

				}
			}
		}

		return std::make_pair(bestWeapon, inRange);
	}

	Weapon* UnitWeaponSet::FindByName(const string& name)
	{
		for(auto weaponIt = _weapons.begin(), end = _weapons.end();
			weaponIt != end; ++weaponIt)
		{
			if((*weaponIt)->GetName().compare(name) == 0)
			{
				return *weaponIt;
			}
		}
		return 0;
	}
}