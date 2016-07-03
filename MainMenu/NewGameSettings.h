#pragma once

#include <Xml.h>
#include <TypeDefs.h>
#include <Array.h>
#include <OgreVector3.h>

namespace FlagRTS
{
	struct NewGameSettings
	{
	public:
		struct PlayerInfo
		{
			int Number;
			int Team;
			string Faction;
			Vector2 StartLocation;
			Vector3 Color;
			bool IsHuman;
			bool IsAI;
		};

		int GameType;
		RefPtr<XmlDocument> MapFile;
		string MapFileDir;
		string ModeName;
		int ResourceMode;
		int VisibilityMode;
		int StartLocMode;
		Array<PlayerInfo> Players;
	};
}