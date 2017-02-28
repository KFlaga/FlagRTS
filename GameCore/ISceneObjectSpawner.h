#pragma once

#include "DataTypes.h"
#include "IGameObject.h"
#include <Event.h>
#include "SpawnInfo.h"

namespace FlagRTS
{
	class SceneObject;
	class SceneObjectDefinition;
	class IGameObjectPool;

	/// Interface for class responsible for spawning/despawning SceneObjects
	/**
	Interface for class responsible for spawning/despawning SceneObjects.
	*/
	class ISceneObjectSpawner
	{
	protected:
		Event<ISceneObjectSpawner*, SceneObject*> _objectCreated;
		Event<ISceneObjectSpawner*, SceneObject*> _objectSpawned;
		Event<ISceneObjectSpawner*, SceneObject*> _objectDespawned;
		Event<ISceneObjectSpawner*, SceneObject*> _objectDestroyed;

	public:
		ISceneObjectSpawner() { }
		virtual ~ISceneObjectSpawner() { }

		/// Creates and spawns SceneObject
		/**
		Creates and loads new SceneObject using supplied definition and spawns it.
		\param objectDefinition definition of object to be spawned
		\param spawnInfo SpawnInfo?
		\param queue if true then object is created but spawned at the end of frame ('false' by default)
		\returns pointer to created, loaded and spawned SceneObject if those operations were successful or 0 otherwise
		*/
		virtual SceneObject* SpawnSceneObject(SceneObjectDefinition* objectDefinition, 
			const SpawnInfo& spawnInfo, bool queue = false) = 0;

		/// Spawns not spawned SceneObject
		/**
		Spawns created and loaded SceneObject. 
		If object is already spawned or is not valid / loaded does nothing.
		\param sceneObject object to be spawned
		\param queue if true then object is spawned at the end of frame ('false' by default)
		*/
		virtual void SpawnSceneObject(SceneObject* sceneObject, const SpawnInfo& spawnInfo, bool queue = false) = 0;

		/// Despawns spawned SceneObject
		/**
		Despawns previously spawned SceneObject. 
		If object is not spawned or is not valid does nothing.
		\param sceneObject object to be despawned
		\param queue if true then object is despawned at the end of frame ('false' by default)
		*/
		virtual void DespawnSceneObject(SceneObject* object, bool queue = true) = 0;

		/// Despawns, unloads and destroys spawned SceneObject
		/**
		Despawns previously spawned SceneObject, unloads and destroys it. 
		If object is not spawned only unloads/destroys. If object is not valid does nothing.
		\param sceneObject object to be despawned
		\param queue if true then object is despawned at the end of frame ('false' by default)
		*/
		virtual void DestroySceneObject(SceneObject* object, bool queue = true) = 0;

		/// Processes queued despawns
		/**
			Processes all queued despawns, then destructions.
			Should be called a the end of frame.
		*/
		virtual void Update(float);

		/**
			Event occurs after SceneObject is fully created and loaded
		*/
		Event<ISceneObjectSpawner*, SceneObject*>& OnSceneObjectCreated() { return _objectCreated; }
		
		/**
			Event occurs after SceneObject is fully spawned
		*/
		Event<ISceneObjectSpawner*, SceneObject*>& OnSceneObjectSpawned() { return _objectCreated; }
		
		/**
			Event occurs before SceneObject is despawned
			(if despawne is queued, it is called on the end of frame)
		*/
		Event<ISceneObjectSpawner*, SceneObject*>& OnSceneObjectDespawned() { return _objectCreated; }
		
		/**
			Event occurs before SceneObject is unloaded and destroyed 
			(if destroy is queued, it is called on the end of frame)
		*/
		Event<ISceneObjectSpawner*, SceneObject*>& OnSceneObjectDestroyed() { return _objectCreated; }
	};
}