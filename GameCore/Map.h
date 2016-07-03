#pragma once

#include "DataTypes.h"
#include <ObjectHandle.h>
#include "SceneObject.h"

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <map>

#include "SpawnInfo.h"
#include "MapTerrain.h"

namespace FlagRTS
{
	// Map is root node of Ogre SceneManager, contains all created SceneObjects
	// and terrain ( does not create them though )
	class Map
	{
	public:
		typedef List<SceneObject*> SceneObjectsMap;

	private:
		Ogre::SceneManager* _ogreSceneMgr;
		Ogre::SceneNode* _mapNode;

		SceneObjectsMap _spawnedObjects;
		RefPtr<MapTerrain> _terrain;

		bool _destroyed; // Prevent multiple calls to DestoryMap()
		
		IntVector2 _tileSize; // Size of one terrain tile in cells
		Vector2 _cellSize; // Size of one terrain cell in units

	public:
		Map(Ogre::SceneManager*);
		~Map();

		Ogre::SceneNode* GetRootNode() { return _mapNode; }
		
		// Adds terrain to map
		// Only one terrain can be added, so it unloads previous one if was set
		void SetTerrain(RefPtr<MapTerrain>);

		// Removes terrain from map
		void ClearTerrain();

		// Builds loaded terrain ( it must be set prior to call )
		void CreateTerrain();

		// Returns added terrain
		const RefPtr<MapTerrain>& GetTerrain() const { return _terrain; }

		// Spawns ready to spawn object 
		// ( attaches to root if detached and calls spawn on it )
		void SpawnObject(SceneObject* object, const SpawnInfo& si);

		// Despawn object from map (detach from scene graph and call Despawn on object )
		// Also despawns all child objects???
		void DespawnObject(SceneObject* object);

		// Despawns all objects and removes terrain
		void DestroyMap();

		// Returns point where ray intersects terrain. Negative values if does not intersect
		Vector3 GetTerrainInterscetion(Ogre::Ray ray) const { return _terrain->GetTerrainInterscetion(ray); }
		// Returns height of terrain on given (x,z) point ( average of 4 closest vertices )
		float GetTerrainHeight(float x, float z) const { return _terrain->GetTerrainHeight(x,z); }
		// Returns height of terrain in one of vertices
		float GetTerrainVertexHeight(uint32 vx, uint32 vy)
		{
			return _terrain->GetTerrainVertexHeight(vx,vy);
		}

		const Vector2& GetTerrainSize() const
		{
			return _terrain->GetTerrainSize();
		}

		const IntVector2& GetTileSize() const { return _tileSize; }
		const Vector2& GetCellSize() const { return _cellSize; }
		
		Array2d<float>& GetFinalHeightMap() { return _terrain->GetFinalHeightMap(); }
		const Array2d<float>& GetFinalHeightMap() const { return _terrain->GetFinalHeightMap(); }
	};
}