#include "SceneObjectSpawner.h"
#include "SceneObject.h"
#include "GameWorld.h"
#include "Map.h"

namespace FlagRTS
{
	SceneObjectSpawner::SceneObjectSpawner(IGameObjectPool* mainObjectPool, 
		Ogre::SceneManager* ogreMgr, Map* map) :
	_mainObjectPool(mainObjectPool),
		_ogreMgr(ogreMgr),
		_map(map)
	{

	}

	SceneObjectSpawner::~SceneObjectSpawner()
	{

	}

	void SceneObjectSpawner::Update(float)
	{
		for(int i = 0; i < _spawnPending.size(); ++i)
		{
			SpawnSceneObject(_spawnPending[i].first, _spawnPending[i].second, false);
		}
		_spawnPending.clear();
		
		for(int i = 0; i < _despawnPending.size(); ++i)
		{
			DespawnSceneObject(_despawnPending[i], false);
		}
		_despawnPending.clear();
		
		for(int i = 0; i < _destroyPending.size(); ++i)
		{
			DestroySceneObject(_despawnPending[i], false);
		}
		_destroyPending.clear();
	}

	SceneObject* SceneObjectSpawner::SpawnSceneObject(SceneObjectDefinition* objectDefinition, 
		const SpawnInfo& spawnInfo, bool queue)
	{
		_ASSERT(objectDefinition != 0);

		SceneObject* object = static_cast<SceneObject*>(
			_mainObjectPool->Create(objectDefinition, NEUTRAL_PLAYERNUM));
		object->LoadResources(_ogreMgr);

		_objectCreated.Fire(this, object);

		object->GetStates().ChangeState(SceneObjectStates::NotSpawned);
		SpawnSceneObject(object, spawnInfo, queue);
	}

	void SceneObjectSpawner::SpawnSceneObject(SceneObject* sceneObject, 
		const SpawnInfo& spawnInfo, bool queue)
	{
		_ASSERT(sceneObject != 0);
		if(queue)
		{
			_spawnPending.push_back(std::make_pair(sceneObject, spawnInfo));
			return;
		}
		_map->SpawnObject(sceneObject, spawnInfo);
		_objectSpawned.Fire(this, sceneObject);
	}

	void SceneObjectSpawner::DespawnSceneObject(SceneObject* object, bool queue)
	{
		_ASSERT(object != 0);
		if(queue)
		{
			_despawnPending.push_back(object);
			return;
		}

		_objectDespawned.Fire(this, object);
		_map->DespawnObject(object);
	}

	void SceneObjectSpawner::DestroySceneObject(SceneObject* object, bool queue)
	{
		_ASSERT(object != 0);
		if(queue)
		{
			_despawnPending.push_back(object);
			_destroyPending.push_back(object);
			return;
		}

		if(object->IsSpawned())
		{
			DespawnSceneObject(object);
		}
		
		_objectDestroyed.Fire(this, object);
		object->UnloadResources(_ogreMgr);
		_mainObjectPool->Destroy(object);
	}
}