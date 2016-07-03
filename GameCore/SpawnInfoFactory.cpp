#include "SpawnInfoFactory.h"

namespace FlagRTS
{
	SpawnInfo* SpawnInfoFactory::Create(XmlNode* soNode)
	{
		SpawnInfo* sinfo = xNew0(SpawnInfo);

		XmlNode* posNode = soNode->first_node("Position");
		Vector3 position;
		if(posNode != 0)
		{
			position.x = std::stof(posNode->first_attribute("x",1,false)->value());
			position.y = std::stof(posNode->first_attribute("y",1,false)->value());
			position.z = std::stof(posNode->first_attribute("z",1,false)->value());
		}

		XmlNode* rotNode = soNode->first_node("Orientation");
		Quaternion rotation;
		if(rotNode != 0)
		{
			rotation.x = std::stof(rotNode->first_attribute("x",1,false)->value());
			rotation.y = std::stof(rotNode->first_attribute("y",1,false)->value());
			rotation.z = std::stof(rotNode->first_attribute("z",1,false)->value());
		}

		bool spawnOnTerrain = XmlUtility::XmlGetBoolIfExists(
			soNode->first_node("SpawnOnTerrain"),"value",true);
		
		sinfo->SetSpawnOnTerrain(spawnOnTerrain);
		sinfo->SetSpawnOrentation(rotation);
		sinfo->SetSpawnPosition(position);

		return sinfo;
	}
}