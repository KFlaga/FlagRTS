#include "MainGameObjectPool.h"
#include "SceneObjectPools.h"
#include "SceneMarkerPools.h"
#include "FXObjectPools.h"
#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	MainGameObjectPool* MainGameObjectPool::GlobalPool = 0;

	MainGameObjectPool::MainGameObjectPool()
	{
		_objPools.insert(GetTypeId<MapDecorator>() ,xNew0(MapDecoratorPool));
		_objPools.insert(GetTypeId<GameCamera>() ,xNew0(GameCameraPool));
		_objPools.insert(GetTypeId<Unit>() ,xNew0(UnitPool));
		_objPools.insert(GetTypeId<Missle>() ,xNew0(MisslePool));

		_objPools.insert(GetTypeId<TerrainProjectionMarker>() ,xNew0(TerrainProjectionMarkerPool));
		_objPools.insert(GetTypeId<ModelMarker>() ,xNew0(ModelMarkerPool));
		_objPools.insert(GetTypeId<BillboardMarker>() ,xNew0(BillboardMarkerPool));

		_objPools.insert(GetTypeId<FXParticles>() ,xNew0(FXParticlesPool));
		_objPools.insert(GetTypeId<FXSound>() ,xNew0(FXSoundPool));
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
		return _objPools.find(objDef->GetFinalType())->Value->Create(objDef, owner);
	}

	void MainGameObjectPool::Destroy(IGameObject* object)
	{
		_objPools.find(object->GetFinalType())->Value->Destroy(object);
	}

	IGameObject* MainGameObjectPool::FindByHandle(ObjectHandle handle)
	{
		IGameObject* object = reinterpret_cast<IGameObject*>(handle.Object);
		return FindByHandle(object->GetFinalType(), handle);
	}
}