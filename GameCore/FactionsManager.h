#pragma once

#include <ArrayMap.h>

namespace FlagRTS
{
	class Faction;

	// Loads all factions and stores info on every one
	// Allows adding of custom factions
	class FactionsManager
	{
	private:
		ArrayMaps<Faction*>::KeyString _factions;

	public:
		FactionsManager();
		~FactionsManager();

		// Adds faction to available factions
		// If faction with this name exists it will be overriden if its pointer != 'faction' 
		// ( and old one destoroyed )
		void AddFaction(Faction* faction);

		// Loads custom faction from given file and adds it
		void LoadFaction(const string& filePath);

		// Returns faction with given name or 0 if not found
		Faction* GetFaction(const string& name)
		{
			auto facIt = _factions.find(name);
			return facIt != _factions.end() ? facIt->Value : 0;
		}
	};
}