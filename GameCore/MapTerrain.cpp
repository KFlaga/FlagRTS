#include "MapTerrain.h"

#include <Ogre\Terrain\OgreTerrain.h>
#include <Ogre\Terrain\OgreTerrainGroup.h>
#include <OgreImage.h>
#include "ArtefixMapTerrainLoader.h"

namespace FlagRTS
{
	MapTerrain::MapTerrain(Ogre::SceneManager* ogreMgr, float cellSize, int tileCellSize) :
		_ogreMgr(ogreMgr), 
		_tgOptions(0), 
		_terrains(0)
	{
		_terrainInfo.SetBaseHeight(0);
		_terrainInfo.SetCellSize(cellSize);
		_terrainInfo.SetTileCellSize(tileCellSize);
		_terrainInfo.SetTileWorldSize((float)tileCellSize*cellSize);
		_terrainInfo.SetTilesX(1);
		_terrainInfo.SetTilesZ(1);
		_terrainInfo.SetHeightScale(200);
		_terrainInfo.SetName("TestMap");
	}

	MapTerrain::~MapTerrain()
	{
		UnloadTerrain();
	}


	void MapTerrain::ReadTerrainInfo(XmlNode* mapNode)
	{
		XmlNode* terrainNode = mapNode->first_node("Terrain");
		XmlNode* lightNode = mapNode->first_node("GlobalLight");

		_terrainInfo.SetName(mapNode->first_attribute("name",4,false)->value());

		CreateGlobalLight(lightNode);
		ParseTerrainInfo(terrainNode);
		ConfigureTerrain();
	}

	void MapTerrain::LoadTerrain()
	{
		_terrainInfo.GetTileset()->LoadTileset();

		// Find if info on terrain is saved. If it is load from file
		// otherwise perform manual load and save it for later reuse
		// FOR NOW DO NOT LOAD FROM FILE, AS IT TAKES MORE TIME THAN CREATING NEW ONE

		//if(!_terrains->LoadTerrainFromFileIfExists())
		//{
			ITerrainLoader* terrainLoader =  new ArtefixTerrainLoader(_terrains, _terrainInfo);
			terrainLoader->LoadTerrain();
			delete terrainLoader;
			//_terrains->SaveTerrainToFile();
		//}

		_terrains->FreeTemporaryResources();
	}

	void MapTerrain::UnloadTerrain()
	{
		if(_tgOptions != 0)
		{
			OGRE_DELETE _tgOptions;
			_tgOptions = 0;
		}
		if(_terrains != 0)
		{
			_terrains->UnloadAllTerrain();
			_terrains = 0;
		}
	}

	void MapTerrain::ParseTerrainInfo(XmlNode* terrainNode)
	{
		// Read info about terrain sizing
		XmlNode* sizeNode = terrainNode->first_node("Size");
		_terrainInfo.SetTilesX(std::stoi(sizeNode->first_attribute("x")->value()));
		_terrainInfo.SetTilesZ(std::stoi(sizeNode->first_attribute("z")->value()));

		XmlNode* bhNode = terrainNode->first_node("BaseHeight");
		_terrainInfo.SetBaseHeight(std::stoi(bhNode->first_attribute("value")->value()));

		XmlNode* hsNode = terrainNode->first_node("HeightScale");
		_terrainInfo.SetHeightScale(std::stoi(hsNode->first_attribute("value")->value()));

		// Read tile set
		XmlNode* tsNode = terrainNode->first_node("Tileset");
		_terrainInfo.SetTileset(RefPtr<Tileset>(xNew6(Tileset,
			tsNode->first_attribute("name",4,false)->value(), // tileset name
			tsNode->first_attribute("texname",7,false)->value(), // texturesname name
			std::stoi(tsNode->first_attribute("count",5,false)->value()), // textures count
			std::stoi(tsNode->first_attribute("size",4,false)->value()), // texture size
			tsNode->first_attribute("ext",3,false)->value(), // textures extension
			"Maps"))); // Resource group with textures from tileset
		// TODO add any resource groups

		_terrainBounds = Vector2((float)(_terrainInfo.GetTileWorldSize() * _terrainInfo.GetTilesX()),
				(float)(_terrainInfo.GetTileWorldSize() * _terrainInfo.GetTilesZ()));
	}

	
		void MapTerrain::CreateGlobalLight(XmlNode* lightNode)
		{
			// Read ambient light color
			XmlNode* ambNode = lightNode->first_node("Ambient");
			Vector3 rgb = XmlUtility::XmlGetRGB(ambNode);
			_ogreMgr->setAmbientLight(Ogre::ColourValue(rgb.x,rgb.y,rgb.z));
 
			// Read sun-light direction
			XmlNode* dirNode = lightNode->first_node("Direction");
			Vector3 lightdir = XmlUtility::XmlGetXYZ(dirNode);

			// Create directional light
			lightdir.normalise();
 
			_globalLight = _ogreMgr->createLight("GlobalLight");
			_globalLight->setType(Ogre::Light::LT_DIRECTIONAL);
			_globalLight->setDirection(lightdir);

			// Read sun-light colors
			XmlNode* colNode = lightNode->first_node("ColorDiffuse");
			rgb = XmlUtility::XmlGetRGB(colNode);
			_globalLight->setDiffuseColour(Ogre::ColourValue(rgb.x,rgb.y,rgb.z));
			
			colNode = lightNode->first_node("ColorSpecular");
			rgb = XmlUtility::XmlGetRGB(colNode);
			_globalLight->setSpecularColour(Ogre::ColourValue(rgb.x,rgb.y,rgb.z));
		}

	void MapTerrain::ConfigureTerrain()
	{
		_tgOptions = OGRE_NEW Ogre::TerrainGlobalOptions();

		_tgOptions->setCastsDynamicShadows(false);
		_tgOptions->setCompositeMapDistance(3000);
		_tgOptions->setLightMapDirection(_globalLight->getDerivedDirection());
		_tgOptions->setCompositeMapAmbient(_ogreMgr->getAmbientLight());
		_tgOptions->setCompositeMapDiffuse(_globalLight->getDiffuseColour());

		_terrains = xNew1(TerrainGrid,_ogreMgr);

		_terrains->SetResourceGroup("Maps");
		_terrains->SetTerrainInfo(_terrainInfo);
	}
}