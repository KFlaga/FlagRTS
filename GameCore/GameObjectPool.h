#pragma once

#include "IGameObjectPool.h"
#include <HashMap.h>
#include <type_traits>
#include "ObjectDefinition.h"

namespace FlagRTS
{
	/// Defualt implementation of IGameObjectPool
	/**
		Creates objects from definition and stores them.
		Does not load them nor creates its components.
	*/
	template<typename ObjType, typename DefType>
	class GameObjectPool : public IGameObjectPool
	{
		DISALLOW_COPY(GameObjectPool);

	protected:
		typedef HashedMap<size_t, ObjType*> ObjectMap;
		ObjectMap _objects;

	public:
		GameObjectPool() { }
		~GameObjectPool()
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				xDelete(objIt->Value);
			}
		}

		void LoadAllResources(Ogre::SceneManager* ogreMgr)
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				objIt->Value->LoadResources(ogreMgr);
			}
		}

		void UnloadAllResources(Ogre::SceneManager* ogreMgr)
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				objIt->Value->UnloadResources(ogreMgr);
			}
		}

		void UpdateAll(float ms)
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				objIt->Value->Update(ms);
			}
		}

		TypeId GetObjectType() const { return GetTypeId<ObjType>(); }
		TypeId GetDefinitionType() const { return GetTypeId<DefType>(); }

		IGameObject* FindByHandle(TypeId objectType, ObjectHandle handle)
		{
			_ASSERT(GetObjectType() == objectType);
			_ASSERT(_objects.find(handle.Object) != _objects.end());
			return _objects[handle.Object];
		}

		IGameObject* Create(ObjectDefinition* objDef, int owner)
		{
			ObjType* newObj(xNew1(ObjType, static_cast<DefType*>(objDef)));
			_objects.insert(newObj->GetHandle().Object, newObj);
			newObj->SetOwner(owner);

			return newObj;
		}

		void Destroy(IGameObject* object)
		{
			_objects.erase(object->GetHandle().Object);
			xDelete(object);
		}

		void DestroyAll()
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				Destroy(objIt->Value);
			}
			_objects.clear();
		}

		//auto GetObjectsBegin() -> decltype(_objects.begin()) { return _objects.begin(); } 
		//auto GetObjectsEnd() -> decltype(_objects.end()) { return _objects.end(); }
	};
}