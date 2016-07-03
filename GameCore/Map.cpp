#include "Map.h"
#include <ErrorCodes.h>
#include "MapDecorator.h"
#include "ArtefixMapTerrainLoader.h"
#include "Unit.h"
#include <ConfigurationManager.h>

namespace FlagRTS
{
	Map::Map(Ogre::SceneManager* ogreMgr) :
		_ogreSceneMgr(ogreMgr),
		_mapNode(ogreMgr->getRootSceneNode())
	{
		// Read some options from config
		auto& mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
		int tilesize = std::stoi(mainConfig->FindOptionValue("TileSize", "Terrain"));
		float cellsize = (float)std::stoi(mainConfig->FindOptionValue("CellSize", "Terrain"));
		_tileSize = IntVector2(tilesize, tilesize);
		_cellSize = Vector2(cellsize, cellsize);
	}

	Map::~Map()
	{
		if(!_destroyed)
			DestroyMap();
	}

	void Map::SetTerrain(RefPtr<MapTerrain> terrain)
	{
		if(_terrain.IsValid())
		{
			ClearTerrain();
		}
		_terrain = terrain;
	}

	void Map::ClearTerrain()
	{
		if(_terrain.IsValid())
		{
			_terrain->UnloadTerrain();
			_terrain = RefPtr<MapTerrain>::InvalidRefPointer();
		}
	}

	void Map::CreateTerrain() 
	{ 
		_terrain->LoadTerrain();
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

		_ogreSceneMgr->destroySceneNode(objNode);

		object->SetSceneNode(0);
	}

	void Map::DestroyMap()
	{
		ClearTerrain();
		_mapNode->removeAndDestroyAllChildren();
	}
}