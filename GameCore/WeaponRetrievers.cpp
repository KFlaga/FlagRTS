#include "WeaponRetrievers.h"
#include <Exception.h>
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "Weapon.h"
#include "Unit.h"
#include "UnitRetrievers.h"

namespace FlagRTS
{
	IObjectRetriever<Weapon*>* WeaponRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Definition") == 0)
		{
			const char* name = XmlUtility::XmlGetString(valueNode, "definition", 5);
			return xNew1(WeaponNewFromDefinitionRetriever, name);
		}
		else if(strcmp(from, "UnitWeaponName") == 0)
		{
			IObjectRetriever<Unit*>* unit = UnitRetrieverFactory::Create(
				valueNode->first_node("Unit"));
			const char* name = XmlUtility::XmlGetString(valueNode, "name", 5);

			return xNew2(UnitWeaponNameRetriever, name, unit);
		}
		else
		{
			CastException((string("Bad from :") + from).c_str());
			return 0;
		}
	}

	WeaponNewFromDefinitionRetriever::operator Weapon*() const
	{
		WeaponDefinition* def = static_cast<WeaponDefinition*>(
			GameInterfaces::GetObjectDefinitionManager()->GetObjectDefinitionByName("Weapon", _defName));

		return xNew1(Weapon, def);
	}

	UnitWeaponNameRetriever::~UnitWeaponNameRetriever()
	{
		xDelete(_unit);
	}

	UnitWeaponNameRetriever::operator Weapon*() const
	{
		return static_cast<Unit*>(*_unit)->GetWeapons()->FindByName(_name);
	}
}