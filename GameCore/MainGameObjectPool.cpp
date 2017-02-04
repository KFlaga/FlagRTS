#include "MainGameObjectPool.h"
#include "GameObjectPool.h"
#include "SceneObjectPools.h"
#include "SceneMarkerPools.h"
#include "FXParticles.h"
#include "FXSound.h"
#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	MainGameObjectPool::MainGameObjectPool()
	{
		_objPools.insert(GetTypeId<MapDecorator>(), xNew0(MapDecoratorPool));
		_objPools.insert(GetTypeId<GameCamera>(), xNew0(GameCameraPool));
		_objPools.insert(GetTypeId<Unit>(), xNew0(UnitPool));
		_objPools.insert(GetTypeId<Missle>(), xNew0(MisslePool));

		_objPools.insert(GetTypeId<TerrainProjectionMarker>(), xNew0(TerrainProjectionMarkerPool));
		_objPools.insert(GetTypeId<ModelMarker>(), xNew0(ModelMarkerPool));
		_objPools.insert(GetTypeId<BillboardMarker>(), xNew0(BillboardMarkerPool));

		typedef GameObjectPool<FXParticles, FXParticlesDefinition> FXPartclesPool;
		_objPools.insert(GetTypeId<FXParticles>(), xNew0(FXPartclesPool));
		typedef GameObjectPool<FXSound, FXSoundDefinition> FXSoundPool;
		_objPools.insert(GetTypeId<FXSound>(), xNew0(FXSoundPool));
	}

	MainGameObjectPool::~MainGameObjectPool()
	{
		for(auto poolIt = _objPools.begin(); poolIt != _objPools.end(); ++poolIt)
		{
			xDelete(poolIt->Value);
		}
		_objPools.clear();
	}

	void MainGameObjectPool::LoadAllResources(Ogre::SceneManager* ogre)
	{
		for(auto poolIt = _objPools.begin(); poolIt != _objPools.end(); ++poolIt)
		{
			poolIt->Value->LoadAllResources(ogre);
		}
	}

	void MainGameObjectPool::UnloadAllResources(Ogre::SceneManager* ogre)
	{
		for(auto poolIt = _objPools.begin(); poolIt != _objPools.end(); ++poolIt)
		{
			poolIt->Value->UnloadAllResources(ogre);
		}
	}

	void MainGameObjectPool::UpdateAll(float ms)
	{
		for(auto poolIt = _objPools.begin(); poolIt != _objPools.end(); ++poolIt)
		{
			poolIt->Value->UpdateAll(ms);
		}
	}

	void MainGameObjectPool::DestroyAll()
	{
		for(auto poolIt = _objPools.begin(); poolIt != _objPools.end(); ++poolIt)
		{
			poolIt->Value->DestroyAll();
		}
	}

	IGameObject* MainGameObjectPool::Create(ObjectDefinition* objDef, int owner)
	{
		return GetObjectPool(objDef->GetFinalType())->Create(objDef, owner);
	}

	void MainGameObjectPool::Destroy(IGameObject* object)
	{
		GetObjectPool(object->GetFinalType())->Destroy(object);
	}

	IGameObject* MainGameObjectPool::FindByHandle(TypeId objectType, ObjectHandle handle)
	{
		return GetObjectPool(objectType)->FindByHandle(objectType, handle);
	}

	IGameObjectPool* MainGameObjectPool::GetObjectPool(TypeId ofType)
	{
		return _objPools.find(ofType)->Value;
	}

	IGameObject* MainGameObjectPool::GetLastCreatedObject(TypeId ofType)
	{
		return GetObjectPool(ofType)->GetLastCreatedObject();
	}
}