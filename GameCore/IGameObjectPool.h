#pragma once

#include <ObjectHandle.h>
#include "IGameObject.h"
#include "DataTypes.h"
#include "ObjectDefinition.h"

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	/// Interface for objects that create/store/update game object
	/**
		Interface for pool of IGameObject final types.
		All instances of those object should be created/destroyed via corresponding pool.
		Created objects are stored and updated via pool
	*/
	class IGameObjectPool
	{
		DISALLOW_COPY(IGameObjectPool);

		IGameObject* _lastCreatedObject;

	public:
		IGameObjectPool() { }
		virtual ~IGameObjectPool() { }

		/// Loads resources for every created object
		virtual void LoadAllResources(Ogre::SceneManager* ogrMgr) = 0;

		/// Unloads resources for every created object
		virtual void UnloadAllResources(Ogre::SceneManager* ogrMgr) = 0;
		
		/// Updates all created objects
		virtual void UpdateAll(float ms) = 0;
		
		/// Gets object last created by this pool
		IGameObject* GetLastCreatedObject()
		{
			return _lastCreatedObject;
		}

		/// Creates object from given definition
		/**
			Creates object from given definition.
			Created objects should have their
			Passed ObjectDefinition must be defintion if same type as this pool.
			\param objDef definition of object that should be created
			\param owner owning player need if object need to have owner
			\returns fully created object if objDef is ok or 0 otherwise
		*/
		virtual IGameObject* Create(ObjectDefinition* objDef, int owner) = 0;
		
		/// Destroys and removes object from pool
		/**
			Destroys and removes object from pool.
			Does not unload object - it should be unloaded externally
		*/
		virtual void Destroy(IGameObject* object) = 0;

		/// Destroys every created object in this pool
		virtual void DestroyAll() = 0;

		/// Retuns IGameObject with given handle and type
		/**
			Searches for and returns pointer to created 'IGameObject' with specified
			TypeId and handle.
			\param objectType TypeId of object
			\param handle handle to object
			\returns object with given handle if found or 0 otherwise
		*/
		virtual IGameObject* FindByHandle(TypeId objectType, ObjectHandle handle) = 0;
	};
}