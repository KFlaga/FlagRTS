#pragma once

#include "IGameObjectPool.h"

namespace FlagRTS
{
	/// Interface for holder of IGameObjectPools
	/**
		Interface for holder of IGameObjectPools.
		All registered object pools are owned by this object and
		are destroyed together with it.
	*/
	class IGameObjectPoolManager
	{
	public:
		IGameObjectPoolManager() { }
		virtual ~IGameObjectPoolManager() { }
		/// Returns IGameObjectPool of given type
		/**
			Searches for and returns pointer to 'IGameObjectPool' registered for specified TypeId
			\param ofType TypeId of game object class we look pool for
		*/
		virtual IGameObjectPool* GetObjectPool(TypeId ofType) = 0;
		
		/// Adds object pool for given type
		/**
			Adds pool for creating given object type.
			If pool for specified type already exists, unregisters it.
			\param ofType TypeId of game object cthis pool creates
		*/
		virtual void RegisterObjectPool(IGameObjectPool* pool, TypeId type) = 0;
		
		/// Removes IGameObjectPool of given type
		/**
			Removes pool registered for given type, destroys all object
			created and xDeletes it.
			If there's no such pool does nothing.
			\param ofType TypeId of registered object pool
		*/
		virtual void UnregisterObjectPool(TypeId type) = 0;
	};
}