#pragma once

#include "KillReason.h"
#include "DataTypes.h"
#include <map>

namespace FlagRTS
{
	namespace KillReasons
	{
		std::map<string, KillReasonType>& GetStateNamesMap()
		{
			static std::map<string, KillReasonType> map;
			map["Unknown"] = Unknown;
			map["Vanished"] = Vanished;
			map["Aborted"] = Aborted;
			map["SelfDestruct"] = SelfDestruct;
			map["FriendlyFire"] = FriendlyFire;
			map["KilledByPlayer"] = KilledByPlayer;
			return map;
		}

		KillReasonType ParseKillReason(const char* name)
		{
			static std::map<string, KillReasonType>& map = GetStateNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return Unknown;
		}
	}
}