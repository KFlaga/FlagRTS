
#include "FactionsManager.h"
#include "Faction.h"
#include <OgreResourceManager.h>

namespace FlagRTS
{
	FactionsManager::FactionsManager()
	{

	}

	FactionsManager::~FactionsManager()
	{
		for(auto facIt = _factions.begin(); facIt != _factions.end(); ++facIt)
		{
			xDelete( facIt->Value );
		}
	}

	void FactionsManager::AddFaction(Faction* faction)
	{
		Faction* oldFaction = GetFaction(faction->GetName());
		if( oldFaction == 0 )
		{
			_factions.insert(faction->GetName(), faction);
		}
		else if( oldFaction != faction )
		{
			_factions[faction->GetName()] = faction;
			xDelete(oldFaction);
		}
	}

	void FactionsManager::LoadFaction(const string& filePath)
	{
		RefPtr<XmlDocument> factionDoc = XmlUtility::XmlDocFromFile(filePath.c_str());

		XmlNode* facNode = factionDoc->first_node("Faction");

		Faction* faction = xNew0(Faction);	
		faction->LoadFromXml(facNode);

		AddFaction(faction);
	}
}