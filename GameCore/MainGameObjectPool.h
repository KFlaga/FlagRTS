#pragma once

#include "IGameObjectPool.h"
#include "IGameObjectPoolManager.h"
#include <TypeId.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	/// Default implementation of global object pool
	/**
	Default implementation of 'IGameObjectPoolManager'.
	Also implements 'IGameObjectPool' - all game object should be created
	via this pool
	*/
	class MainGameObjectPool : public IGameObjectPool, public IGameObjectPoolManager
	{
		DISALLOW_COPY(MainGameObjectPool);

	protected:
		ArrayMap<TypeId, IGameObjectPool*> _gameObjectPools;
		ArrayMap<TypeId, IComponentPool*> _componentPools;

	public:
		MainGameObjectPool();
		~MainGameObjectPool();

		void LoadAllResources(Ogre::SceneManager*);
		void UnloadAllResources(Ogre::SceneManager*);
		void UpdateAll(float ms);

		IGameObject* FindByHandle(TypeId objectType, ObjectHandle handle);

		//IGameObject* GetLastCreatedObject(TypeId ofType);

		IGameObject* Create(ObjectDefinition* objDef, int owner);

		void Destroy(IGameObject* object);

		void DestroyAll();

		IGameObjectPool* GetGameObjectPool(TypeId ofType);

		void RegisterGameObjectPool(TypeId type, IGameObjectPool* pool);

		void UnregisterGameObjectPool(TypeId type);

		IComponentPool* GetComponentPool(TypeId ofType);

		void RegisterComponentPool(TypeId type, IComponentPool* pool);

		void UnregisterComponentPool(TypeId type);
	};
}