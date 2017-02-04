#pragma once

#include "DataTypes.h"
#include <ObjectHandle.h>
#include "SceneObject.h"

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <map>

#include "SpawnInfo.h"
#include "TerrainBase.h"

namespace FlagRTS
{
	// Map is root node of Ogre SceneManager, contains all created SceneObjects
	// and terrain ( does not create them though )
	class Map
	{
	public:
		typedef List<SceneObject*> SceneObjectsMap;

	private:
		Ogre::SceneManager* _ogreMgr;
		Ogre::SceneNode* _mapNode;

		SceneObjectsMap _spawnedObjects;
		TerrainBase* _terrain;
		Ogre::Light* _globalLight;

		bool _destroyed; // Prevent multiple calls to DestoryMap()

		int _tileSize;
		float _cellSize;

	public:
		Map(Ogre::SceneManager*);
		~Map();

		Ogre::SceneNode* GetRootNode() { return _mapNode; }
		
		// Spawns ready to spawn object 
		// ( attaches to root if detached and calls spawn on it )
		void SpawnObject(SceneObject* object, const SpawnInfo& si);

		// Despawn object from map (detach from scene graph and call Despawn on object )
		// Also despawns all child objects???
		void DespawnObject(SceneObject* object);

		// Despawns all objects and removes terrain
		void DestroyMap();

		// Creates and initializes terrain with info from map node
		// Only one terrain can be added, so it unloads previous one if was set
		void InitTerrain(XmlNode* mapNode);

		// Removes terrain from map
		void ClearTerrain();

		// Loads and spawns terrain
		void LoadAndSpawnTerrain();

		// Returns added terrain
		TerrainBase* GetTerrain() { return _terrain; }

		// Returns point where ray intersects terrain. Negative values if does not intersect
		Vector3 GetTerrainInterscetion(Ogre::Ray ray) const { return _terrain->GetTerrainInterscetion(ray); }
		// Returns height of terrain on given (x,z) point ( average of 4 closest vertices )
		float GetTerrainHeight(float x, float z) const { return _terrain->GetTerrainHeight(x,z); }
		// Returns height of terrain in one of vertices
		float GetTerrainVertexHeight(uint32 vx, uint32 vy)
		{
			return _terrain->GetTerrainVertexHeight(vx,vy);
		}

		const Vector2& GetWorldSizeOfTerrain() const
		{
			return _terrain->GetTerrainInfo().GetWorldSize();
		}

		int GetCellCountInTerrainTile() const 
		{ 
			return _tileSize;
		}

		float GetWorldSizeOfCell() const 
		{ 
			return _cellSize; 
		}
		
		Array2d<float>& GetFinalHeightMap() { return _terrain->GetFinalHeightMap(); }
		const Array2d<float>& GetFinalHeightMap() const { return _terrain->GetFinalHeightMap(); }
		
		// Gets global light
		Ogre::Light* GetGlobalLight() const { return _globalLight; }

	private:
		void CreateGlobalLight(XmlNode* lightNode);
	};
}