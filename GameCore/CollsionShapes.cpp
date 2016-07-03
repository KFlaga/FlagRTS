#include "CollisionShapes.h"
#include <map>
#include "DataTypes.h"

namespace FlagRTS
{
	namespace CollisionShapes
	{
		std::map<string, CollisionShapeType>& GetStateNamesMap()
		{
			static std::map<string, CollisionShapeType> map;
			map["None"] = None;
			map["Box"] = Box;
			map["Circle"] = Circle;
			map["Footprint"] = Footprint;
			map["Box3d"] = Box3d;
			map["Sphere"] = Sphere;
			map["Cone2d"] = Cone2d;
			map["Cone3d"] = Cone3d;
			map["Hull2d"] = Hull2d;
			map["Hull3d"] = Hull3d;
			return map;
		}

		CollisionShapeType ParseCollisionShapeType(const char* name)
		{
			static std::map<string, CollisionShapeType>& map = GetStateNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return None;
		}
	}
}