#pragma once

#include <ObjectHandle.h>
#include "IGameObject.h"
#include "DataTypes.h"
#include "ObjectDefinition.h"
#include <HashMap.h>

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	class IObjectStateSupplier;

	class IGameObjectPool
	{
	IGameObject* _lastCreatedObject;

	public:
		IGameObjectPool() { }
		virtual ~IGameObjectPool() { }

		// Loads resources ( mainly Ogre's ) for every created object
		virtual void LoadAllResources(Ogre::SceneManager*) = 0;
		// UnLoads resources ( mainly Ogre's ) for every created object
		virtual void UnloadAllResources(Ogre::SceneManager*) = 0;
		// Updates all created objects, if applicable
		virtual void UpdateAll(float ms) = 0;
		// Gets object from pool with given handle
		virtual IGameObject* FindByHandle(ObjectHandle handle) = 0;
		// Gets object last created by this pool
		IGameObject* GetLastCreatedObject()
		{
			return _lastCreatedObject;
		}

		// Creates object from given definition and loads its resources
		virtual IGameObject* Create(ObjectDefinition* objDef, int owner) = 0;
		// Unloads, destroys and removes object from pool
		virtual void Destroy(IGameObject* object) = 0;
		// Unloads, destroys and removes object from pool
		virtual void Destroy(ObjectHandle object)
		{
			Destroy(FindByHandle(object));
		}

		virtual void DestroyAll() = 0;

		virtual void RegisterObjectStateSupplier(const char* objectClassName, 
			IObjectStateSupplier* supplier)
		{ }
		virtual void UnregisterObjectStateSupplier(const char* objectClassName) { }
	};

	template<typename ObjType, typename DefType>
	class IGameObjectPoolCast : public IGameObjectPool
	{
	protected:
		typedef HashedMap<size_t, ObjType*> ObjectMap;
		ObjectMap _objects;

	public:
		~IGameObjectPoolCast()
		{
			for(auto objIt = _objects.begin(); objIt != _objects.end(); ++objIt)
			{
				xDelete(objIt->Value);
			}
		}

		TypeId GetObjectType() const { return GetTypeId<ObjType>(); }
		TypeId GetDefinitionType() const { return GetTypeId<DefType>(); }

		ObjType* FindByHandleCast(ObjectHandle handle)
		{
			return _objects[handle.Object];
		}

		ObjType* GetLastCreatedObjectCast()
		{
			return static_cast<ObjType*>(GetLastCreatedObject());
		}
		
		IGameObject* Create(ObjectDefinition* objDef, int owner)
		{
			return static_cast<IGameObject*>(CreateCast(static_cast<DefType*>(objDef), owner));
		}

		virtual ObjType* CreateCast(DefType* objDef, int owner) = 0;

		
		void Destroy(IGameObject* object)
		{
			DestroyCast(static_cast<ObjType*>(object));
		}

		void Destroy(ObjectHandle object)
		{
			DestroyCast(FindByHandleCast(object));
		}

		virtual void DestroyCast(ObjType* object) = 0;

		auto GetObjectsBegin() -> decltype(_objects.begin()) { return _objects.begin(); } 
		auto GetObjectsEnd() -> decltype(_objects.end()) { return _objects.end(); }
	};
}