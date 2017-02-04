#include "TerrainInfo.h"
#include <Xml.h>

namespace FlagRTS
{
	TerrainInfo::TerrainInfo() { }

	TerrainInfo::TerrainInfo(float cellSize, int cellsInTile) :
		_cellSize(cellSize),
		_tileCellSize(cellsInTile)
	{ 
		SetWorldSizeOfTile(cellSize * cellsInTile);
	}

	void TerrainInfo::ReadFromXml(XmlNode* terrainNode)
	{
		// Read info about terrain sizing
		XmlNode* sizeNode = terrainNode->first_node("Size");
		SetTilesX(std::stoi(sizeNode->first_attribute("x")->value()));
		SetTilesZ(std::stoi(sizeNode->first_attribute("z")->value()));

		XmlNode* bhNode = terrainNode->first_node("BaseHeight");
		SetBaseHeight(std::stoi(bhNode->first_attribute("value")->value()));

		XmlNode* hsNode = terrainNode->first_node("HeightScale");
		SetHeightScale(std::stoi(hsNode->first_attribute("value")->value()));

		SetWorldOffset(XmlUtility::XmlGetXYZIfExists(
			terrainNode->first_node("Offset"), Vector3(0.0f, 0.0f, 0.0f)));

		// Read tile set
		XmlNode* tsNode = terrainNode->first_node("Tileset");
		SetTileset(RefPtr<Tileset>(xNew6(Tileset,
			tsNode->first_attribute("name",4,false)->value(), // tileset name
			tsNode->first_attribute("texname",7,false)->value(), // texturesname name
			std::stoi(tsNode->first_attribute("count",5,false)->value()), // textures count
			std::stoi(tsNode->first_attribute("size",4,false)->value()), // texture size
			tsNode->first_attribute("ext",3,false)->value(), // textures extension
			"Maps"))); // Resource group with textures from tileset

		// TODO add any resource groups

		SetWorldSize(Vector2((float)(GetWorldSizeOfTile() * GetTilesX()),
			(float)(GetWorldSizeOfTile() * GetTilesZ())));
	}
}
