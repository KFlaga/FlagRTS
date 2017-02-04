#include "Map.h"
#include <ErrorCodes.h>
#include "Unit.h"
#include <ConfigurationManager.h>

#include "OgreTerrain.h"

namespace FlagRTS
{
	Map::Map(Ogre::SceneManager* ogreMgr) :
		_ogreMgr(ogreMgr),
		_mapNode(ogreMgr->getRootSceneNode()),
		_terrain(0)
	{
		// Read some options from config
		auto& mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
		_tileSize = std::stoi(mainConfig->FindOptionValue("TileSize", "Terrain"));
		_cellSize = (float)std::stoi(mainConfig->FindOptionValue("CellSize", "Terrain"));
	}

	Map::~Map()
	{
		if(!_destroyed)
			DestroyMap();
	}

	void Map::InitTerrain(XmlNode* mapNode)
	{
		ClearTerrain();

		TerrainInfo terrainInfo(_cellSize, _tileSize);
		terrainInfo.SetName(mapNode->first_attribute("name")->value()); 

		terrainInfo.ReadFromXml(mapNode->first_node("Terrain"));

		CreateGlobalLight(mapNode->first_node("GlobalLight"));
		terrainInfo.SetGlobalLight(_globalLight);

		OgreTerrain* ogreTerrain = xNew1(OgreTerrain, _ogreMgr);
		ogreTerrain->Prepare(terrainInfo);

		_terrain = ogreTerrain;
	}

	void Map::ClearTerrain()
	{
		if(_terrain != 0)
		{
			_terrain->DespawnAndUnload();
			_terrain = 0;
		}
	}

	void Map::LoadAndSpawnTerrain() 
	{ 
		_terrain->LoadAndSpawn();
	}

	void Map::SpawnObject(SceneObject* object, const SpawnInfo& si)
	{
		Vector3 spawnPos = si.GetSpawnPosition();
		if(si.GetSpawnOnTerrain() == true)
		{
			spawnPos.y = GetTerrainHeight(spawnPos.x, spawnPos.z);
		}

		if( object->IsIndependent() )
		{
			SceneNode* objNode = _mapNode->createChildSceneNode(
				spawnPos, si.GetSpawnOrientation() );
			
			object->SetSceneNode(objNode);
		}
		else
		{
			// If object is not independent it should be attached to other object
			// so only create scene node, but not attach it
			SceneNode* objNode = _mapNode->createChildSceneNode(
				spawnPos, si.GetSpawnOrientation() );
			_mapNode->removeChild(objNode);

			object->SetSceneNode(objNode);
		}

		object->Spawn();
	}

	void Map::DespawnObject(SceneObject* object)
	{
		SceneNode* objNode = object->GetSceneNode();
		object->Despawn();

		if( objNode->getParent() != 0 )
		{
			objNode->getParent()->removeChild(objNode);
		}

		_ogreMgr->destroySceneNode(objNode);

		object->SetSceneNode(0);
	}

	void Map::DestroyMap()
	{
		ClearTerrain();
		_mapNode->removeAndDestroyAllChildren();
	}

	
	void Map::CreateGlobalLight(XmlNode* lightNode)
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
}