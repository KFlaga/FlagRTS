#pragma once

#include "IGameObjectPool.h"

namespace FlagRTS
{
	/// Interface for holder of IGameObjectPools and IComponentPools
	/**
		Interface for holder of IGameObjectPools and IComponentPools.
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
		virtual IGameObjectPool* GetGameObjectPool(TypeId ofType) = 0;
		
		/// Adds object pool for given type
		/**
			Adds pool for creating given object type.
			If pool for specified type already exists, unregisters it.
			\param ofType TypeId of game object cthis pool creates
		*/
		virtual void RegisterGameObjectPool(TypeId type, IGameObjectPool* pool) = 0;
		
		/// Removes IGameObjectPool of given type
		/**
			Removes pool registered for given type, destroys all object
			created and xDeletes it.
			If there's no such pool does nothing.
			\param ofType TypeId of registered object pool
		*/
		virtual void UnregisterGameObjectPool(TypeId type) = 0;
		
		/// Returns IComponentPool of given type
		/**
			Searches for and returns pointer to 'IGameObjectPool' registered for specified TypeId
			\param ofType TypeId of game object class we look pool for
		*/
		virtual IComponentPool* GetComponentPool(TypeId ofType) = 0;
		
		/// Adds IComponentPool for given type
		/**
			Adds pool for creating given object type.
			If pool for specified type already exists, unregisters it.
			\param ofType TypeId of game object cthis pool creates
		*/
		virtual void RegisterComponentPool(TypeId type, IComponentPool* pool) = 0;
		
		/// Removes IComponentPool of given type
		/**
			Removes pool registered for given type, destroys all object
			created and xDeletes it.
			If there's no such pool does nothing.
			\param ofType TypeId of registered object pool
		*/
		virtual void UnregisterComponentPool(TypeId type) = 0;
	};
}