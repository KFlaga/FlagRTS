#pragma once

#include "DataTypes.h"
#include <Array.h>
#include "Player.h"
#include "Resources.h"
#include <Xml.h>

namespace FlagRTS
{
	// Info about map / settings etc that is 
	// currently played
	struct InGameSettings
	{
		RefPtr<XmlDocument> MapFile; // not header .map file, but actual map data
		RefPtr<XmlDocument> ScriptFile;

		string GameModeName;
		Array<ResourceSetting> ResourceSettings;
		Array<Player*> Players;
		Array<string> ModsUsed;
		bool FogOfWarEnabled;
		bool ShroudEnabled;
		bool StartLocationsRandom;
		int ClientPlayer;
	};
}