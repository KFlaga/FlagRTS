#include "MainGameObjectPool.h"

#include "GameObjectPool.h"
#include "ComponentPool.h"

#include "SceneObjectPools.h"
#include "SceneMarkerPools.h"
#include "FXParticles.h"
#include "FXSound.h"
#include <Memory.h>
#include <Exception.h>

#include "SelectionComponent.h"
#include "MinimapComponent.h"

namespace FlagRTS
{
	MainGameObjectPool::MainGameObjectPool()
	{
		RegisterGameObjectPool(GetTypeId<MapDecorator>(), xNew0(MapDecoratorPool));
		RegisterGameObjectPool(GetTypeId<GameCamera>(), xNew0(GameCameraPool));
		RegisterGameObjectPool(GetTypeId<Unit>(), xNew0(UnitPool));
		RegisterGameObjectPool(GetTypeId<Missle>(), xNew0(MisslePool));

		RegisterGameObjectPool(GetTypeId<TerrainProjectionMarker>(), xNew0(TerrainProjectionMarkerPool));
		RegisterGameObjectPool(GetTypeId<ModelMarker>(), xNew0(ModelMarkerPool));
		RegisterGameObjectPool(GetTypeId<BillboardMarker>(), xNew0(BillboardMarkerPool));

		typedef GameObjectPool<FXParticles, FXParticlesDefinition> FXPartclesPool;
		RegisterGameObjectPool(GetTypeId<FXParticles>(), xNew0(FXPartclesPool));
		typedef GameObjectPool<FXSound, FXSoundDefinition> FXSoundPool;
		RegisterGameObjectPool(GetTypeId<FXSound>(), xNew0(FXSoundPool));
		
		typedef ComponentPool<SelectionComponent, SelectionComponentDefinition> SelectionComponentPool;
		RegisterComponentPool(GetTypeId<SelectionComponent>(), xNew0(SelectionComponentPool));
		typedef ComponentPool<MinimapComponent, MinimapComponentDefinition> MinimapComponentPool;
		RegisterComponentPool(GetTypeId<SelectionComponent>(), xNew0(MinimapComponentPool));
	}

	MainGameObjectPool::~MainGameObjectPool()
	{
		DestroyAll();

		for(auto poolIt = _componentPools.begin(); poolIt != _componentPools.end(); ++poolIt)
		{
			xDelete(poolIt->Value);
		}
		_componentPools.clear();

		for(auto poolIt = _gameObjectPools.begin(); poolIt != _gameObjectPools.end(); ++poolIt)
		{
			xDelete(poolIt->Value);
		}
		_gameObjectPools.clear();
	}

	void MainGameObjectPool::LoadAllResources(Ogre::SceneManager* ogre)
	{
		for(auto poolIt = _gameObjectPools.begin(); poolIt != _gameObjectPools.end(); ++poolIt)
		{
			// Components are loaded inside IGameObjects (for now at least)
			poolIt->Value->LoadAllResources(ogre);
		}
	}

	void MainGameObjectPool::UnloadAllResources(Ogre::SceneManager* ogre)
	{
		for(auto poolIt = _gameObjectPools.begin(); poolIt != _gameObjectPools.end(); ++poolIt)
		{
			// Components are unloaded inside IGameObjects (for now at least)
			poolIt->Value->UnloadAllResources(ogre);
		}
	}

	void MainGameObjectPool::UpdateAll(float ms)
	{
		for(auto poolIt = _gameObjectPools.begin(); poolIt != _gameObjectPools.end(); ++poolIt)
		{
			// Components are updates inside IGameObjects (for now at least)
			poolIt->Value->UpdateAll(ms);
		}
	}

	void MainGameObjectPool::DestroyAll()
	{
		// Destroy all components, then game objects
		for(auto poolIt = _componentPools.begin(); poolIt != _componentPools.end(); ++poolIt)
		{
			poolIt->Value->DestroyAll();
		}

		for(auto poolIt = _gameObjectPools.begin(); poolIt != _gameObjectPools.end(); ++poolIt)
		{
			// Components are not destroyed inside IGameObjects (for now at least)
			poolIt->Value->DestroyAll();
		}
	}

	IGameObject* MainGameObjectPool::Create(ObjectDefinition* objDef, int owner)
	{
		// 1) Create game object
		IGameObject* object = GetGameObjectPool(objDef->GetFinalType())->Create(objDef, owner);
		// 2) Load all components
		Array<ObjectDefinition*> comps = objDef->GetComponentDefinitions();
		for(int i = 0; i < comps.size(); ++i)
		{
			ObjectDefinition* compDef = comps[i];
			IObjectComponent* component = GetComponentPool(compDef->GetFinalType())->Create(compDef, object);
			object->AddComponent(compDef->GetFinalType(), component);
		}
	}

	void MainGameObjectPool::Destroy(IGameObject* object)
	{
		// 1) Destroy all components
		auto comps = object->GetAllComponents();
		for(int i = 0; i < comps.size(); ++i)
		{
			IObjectComponent* component = comps[i];
			GetComponentPool(component->GetDefinition()->GetFinalType())->Destroy(component);
		}
		// 2) Destory object
		GetGameObjectPool(object->GetFinalType())->Destroy(object);
	}

	IGameObject* MainGameObjectPool::FindByHandle(TypeId objectType, ObjectHandle handle)
	{
		return GetGameObjectPool(objectType)->FindByHandle(objectType, handle);
	}

	IGameObjectPool* MainGameObjectPool::GetGameObjectPool(TypeId ofType)
	{
		_ASSERT(_gameObjectPools.find(ofType) != _gameObjectPools.end());
		return _gameObjectPools.find(ofType)->Value;
	}

	void MainGameObjectPool::RegisterGameObjectPool(TypeId type, IGameObjectPool* pool)
	{
		auto poolIt = _gameObjectPools.find(type);
		if(poolIt != _gameObjectPools.end())
		{
			poolIt->Value->DestroyAll();
			xDelete(poolIt->Value);
		}

		_gameObjectPools[type] = pool;
	}

	void MainGameObjectPool::UnregisterGameObjectPool(TypeId type)
	{
		auto poolIt = _gameObjectPools.find(type);
		if(poolIt != _gameObjectPools.end())
		{
			poolIt->Value->DestroyAll();
			xDelete(poolIt->Value);
		}
	}

	IComponentPool* MainGameObjectPool::GetComponentPool(TypeId ofType)
	{
		_ASSERT(_componentPools.find(ofType) != _componentPools.end());
		return _componentPools.find(ofType)->Value;
	}

	void MainGameObjectPool::RegisterComponentPool(TypeId type, IComponentPool* pool)
	{
		auto poolIt = _componentPools.find(type);
		if(poolIt != _componentPools.end())
		{
			poolIt->Value->DestroyAll();
			xDelete(poolIt->Value);
		}

		_componentPools[type] = pool;
	}

	void MainGameObjectPool::UnregisterComponentPool(TypeId type)
	{
		auto poolIt = _componentPools.find(type);
		if(poolIt != _componentPools.end())
		{
			poolIt->Value->DestroyAll();
			xDelete(poolIt->Value);
		}
	}
}