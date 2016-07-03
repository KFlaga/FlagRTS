#include "CollisionGroup.h"
#include <map>
#include "DataTypes.h"

namespace FlagRTS
{
	namespace CollisionGroups
	{
		std::map<string, CollisionGroupType>& GetStateNamesMap()
		{
			static std::map<string, CollisionGroupType> map;
			map["None"] = None;
			map["Ground"] = Ground;
			map["Air"] = Air;
			map["Naval"] = Naval;
			map["Building"] = Building;
			map["Missle"] = Missle;
			return map;
		}

		CollisionGroupType ParseCollisionGroupType(const char* name)
		{
			static std::map<string, CollisionGroupType>& map = GetStateNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return None;
		}
	}
}