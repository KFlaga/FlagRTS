#pragma once

#include "IGameObjectPool.h"
#include <HashMap.h>
#include <type_traits>
#include "ObjectDefinition.h"

namespace FlagRTS
{

	/// Defualt implementation of IGameObjectPool
	/**

	*/
	template<typename ObjType, 
		typename DefType = typename std::enable_if<
		std::is_base_of<ObjectDefinition, DefType>::value>::type> // Check if DefType is ObjectDefinition
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
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
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
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
			{
				objIt->Value->Update(ms);
			}
		}

		TypeId GetObjectType() const { return GetTypeId<ObjType>(); }
		TypeId GetDefinitionType() const { return GetTypeId<DefType>(); }

		IGameObject* FindByHandle(TypeId objectType, ObjectHandle handle)
		{
			_ASSERT(GetObjectType() == objectType);
			return _objects[handle.Object];
		}

		IGameObject* Create(ObjectDefinition* objDef, int owner)
		{
			return CreateCast(static_cast<DefType*>(objDef), owner);
		}

		virtual ObjType* CreateCast(DefType* objDef, int owner)
		{
			ObjType* newObj(xNew1(ObjType, objDef));
			_objects.insert(newObj->GetHandle().Object, newObj);

			return newObj;
		}

		void Destroy(IGameObject* object)
		{
			_objects.erase(object->GetHandle().Object);
			xDelete(object);
		}

		void DestroyAll()
		{
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
			{
				Destroy(objIt->Value);
			}
			_objects.clear();
		}

		//auto GetObjectsBegin() -> decltype(_objects.begin()) { return _objects.begin(); } 
		//auto GetObjectsEnd() -> decltype(_objects.end()) { return _objects.end(); }
	};
}