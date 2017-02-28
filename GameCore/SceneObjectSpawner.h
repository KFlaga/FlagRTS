#pragma once

#include "ISceneObjectSpawner.h"
#include "IObjectDefinitionManager.h"
#include "IGameObjectPool.h"
#include "SpawnInfo.h"
#include <Array.h>

namespace FlagRTS
{
	class Map;

	class SceneObjectSpawner : public ISceneObjectSpawner
	{
		IGameObjectPool* _mainObjectPool;
		Ogre::SceneManager* _ogreMgr;
		Map* _map;
		
		Array<std::pair<SceneObject*, SpawnInfo>> _spawnPending;
		Array<SceneObject*> _despawnPending;
		Array<SceneObject*> _destroyPending;

	public:
		SceneObjectSpawner(IGameObjectPool* mainObjectPool, 
			Ogre::SceneManager* ogreMgr, 
			Map* map);
		~SceneObjectSpawner();

		void Update(float);

		SceneObject* SpawnSceneObject(SceneObjectDefinition* objectDefinition, 
			const SpawnInfo& spawnInfo, bool queue = false);
		void SpawnSceneObject(SceneObject* sceneObject, const SpawnInfo& spawnInfo, bool queue = false);
		void DespawnSceneObject(SceneObject* object, bool queue = true);
		void DestroySceneObject(SceneObject* object, bool queue = true);
	};
}