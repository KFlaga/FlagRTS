#pragma once

#include "SpawnInfo.h"
#include <Xml.h>

namespace FlagRTS
{
	class SpawnInfoFactory
	{
	public:
		SpawnInfoFactory() { }
		~SpawnInfoFactory() { }

		// Creates SpawnInfo from info on SceneObject node from map file
		SpawnInfo* Create(XmlNode* soNode);
	};
}