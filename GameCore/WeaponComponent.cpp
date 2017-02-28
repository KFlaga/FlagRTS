#include "WeaponsComponent.h"
#include "Weapon.h"
#include "Unit.h"

#include "GameWorld.h"
#include "IObjectDefinitionManager.h"

namespace FlagRTS
{
	WeaponComponentDefinition::WeaponComponentDefinition(XmlNode* componentNode)
	{
		SetFinalType(GetTypeId<WeaponComponent>());
		SetFinalTypeName("WeaponComponent");

		/*
		<Component type="WeaponComponent">
		     <Weapon definition="Weapon_RobotSmash" active="true"/>
		     <Weapon definition="Weapon_BigGun" active="true"/>
		</Component>
		*/

		// Load all weapon definitions
		XmlNode* weaponNode = componentNode->first_node("Weapon");
		while(weaponNode != 0)
		{
			bool isActive = XmlUtility::XmlGetBool(weaponNode, "active");

			string weaponName = XmlUtility::XmlGetString(weaponNode, "definition");

			auto getDefinition = [this, weaponName, isActive](IObjectDefinitionManager* mgr) 
			{
				_weaponDefinitions.push_back(static_cast<WeaponDefinition*>(
					mgr->GetObjectDefinitionByName(weaponName, "Weapon")));
				_activeWeapons.push_back(isActive);
			};
			typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
			GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
				xNew1(DefinitionsLoadedHandler, getDefinition);

			weaponNode = weaponNode->next_sibling();
		}
	}

	WeaponComponentDefinition::~WeaponComponentDefinition()
	{

	}

	WeaponComponent::WeaponComponent(ObjectDefinition* definition, IGameObject* owner) :
		IObjectComponent(definition, owner)
	{
		_ASSERT(definition->GetFinalType() == GetTypeId<WeaponComponent>());
		_ASSERT(owner->GetFinalType() == GetTypeId<Unit>());
		SetFinalType(GetTypeId<WeaponComponent>());

		auto compDef = GetDefinition<WeaponComponentDefinition>();
		_weapons.reserve(compDef->GetWeaponDefinitions().size());
		// Create weapons
		for(int i = 0; i < compDef->GetWeaponDefinitions().size(); ++i)
		{
			Weapon* weapon =  xNew1(Weapon, compDef->GetWeaponDefinitions()[i]);
			weapon->SetIsActive(compDef->GetActiveWeapon()[i]);
			weapon->SetOwner(GetOwner<Unit>());
			_weapons.push_back(weapon);
		}
	}

	WeaponComponent::~WeaponComponent()
	{
		for(unsigned int i = 0; i < _weapons.size(); ++i)
		{
			xDelete(_weapons[i]);
		}
	}

	void WeaponComponent::Update(float ms)
	{
		for(auto weaponIt = _weapons.begin(), end = _weapons.end();
			weaponIt != end; ++weaponIt)
		{
			(*weaponIt)->Update(ms);
		}
	}

	std::pair<Weapon*, bool> WeaponComponent::GetWeaponForTarget(CommandTarget* target)
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
				if(range >= target->GetTargetUnit()->GetDistanceToOtherObject(
					GetOwner<Unit>()))
				{
					inRange = true;
					// Get best range weapon for now

				}
			}
		}

		return std::make_pair(bestWeapon, inRange);
	}

	Weapon* WeaponComponent::FindByName(const string& name)
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