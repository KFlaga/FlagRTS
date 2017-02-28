#pragma once

#include "IGameObjectPool.h"
#include <HashMap.h>
#include <type_traits>
#include "ObjectDefinition.h"

namespace FlagRTS
{
	/// Defualt implementation of IComponentPool
	/**
		Creates objects from definition and stores them.
		Does not load them.
	*/
	template<typename ObjType, typename DefType>
	class ComponentPool : public IComponentPool
	{
		DISALLOW_COPY(ComponentPool);

	protected:
		typedef HashedMap<size_t, ObjType*> ObjectMap;
		ObjectMap _objects;

	public:
		ComponentPool() { }
		~ComponentPool()
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
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
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

		IObjectComponent* FindByHandle(TypeId objectType, ObjectHandle handle)
		{
			_ASSERT(GetObjectType() == objectType);
			return _objects[handle.Object];
		}

		IObjectComponent* Create(ObjectDefinition* objDef, IGameObject* owner)
		{
			ObjType* newObj(xNew2(ObjType, static_cast<DefType*>(objDef), owner));
			_objects.insert(newObj->GetHandle().Object, newObj);

			return newObj;
		}

		void Destroy(IObjectComponent* object)
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
	};
}