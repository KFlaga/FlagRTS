#pragma once

#include "DataTypes.h"
#include <ArrayMap.h>
#include "IObjectComponent.h"

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	/// Base class for objects created in game
	/**
		Base class for objects created in game 
		Should be created and stored in dedicated factories/pools.
		May only be created after GameWorld is constructed and its global instance set and
		all should be destroyed before destroing GameWorld.

		Every object stores its handle and TypeId of its final type.
		Final TypeId should be set in constructors of sub classes.
		ObjectHandle is set in IGameObject constructor and cannot be
		overriden. ObjectHandle::Object serves as instance unique id.
		
		IGameObject have empty Update and Load/Unload methods, sub class is not
		required to support those operations (placed here for convenience).
		
		ADD IOBJECTCOMPONENT DESC.
	*/
	class IGameObject
	{
		DISALLOW_COPY(IGameObject);

	protected:
		ArrayMap<TypeId, IObjectComponent*> _components; /// All components this object consist of
		ObjectHandle _handle; // Unique handle to this object (Object part is its address, Type part is final TypeId)
		int _owner; // Player owning this object (if object type support it)

	public:
		IGameObject();
		virtual ~IGameObject();

		ObjectHandle GetHandle() const { return _handle; }

		void SetFinalType(TypeId type) { _handle.Type; }
		TypeId GetFinalType() const { return _handle.Type; }

		virtual void Update(float ms);
		virtual void LoadResources(Ogre::SceneManager* ogreMgr);
		virtual void UnloadResources(Ogre::SceneManager* ogreMgr);

		virtual void SetOwner(int owner) { _owner = owner; }
		int GetOwner() const { return _owner; }

		IObjectComponent* FindComponent(TypeId componentId)
		{
			auto compIt = _components.find(componentId);
			return compIt != _components.end() ? compIt->Value : 0;
		}
		
		template<typename CompType>
		CompType* FindComponent()
		{
			auto compIt = _components.find(GetTypeId<CompType>());
			return compIt != _components.end() ? 
				static_cast<CompType*>(compIt->Value) : 0;
		}

		void AddComponent(TypeId componentId, IObjectComponent* component)
		{
			_components[componentId] = component;
		}
		
		template<typename CompType>
		void AddComponent(CompType* component)
		{
			_components[GetTypeId<CompType>()] = component;
		}
		
		void RemoveComponent(TypeId componentId)
		{
			auto compIt = _components.find(componentId);
			if(compIt != _components.end())
			{
				xDelete(compIt->Value);
				_components.erase(compIt);
			}
		}

		template<typename CompType>
		void RemoveComponent()
		{
			auto compIt = _components.find(GetTypeId<CompType>());
			if(compIt != _components.end())
			{
				xDelete(compIt->Value);
				_components.erase(compIt);
			}
		}

		const ArrayMap<TypeId, IObjectComponent*>& GetAllComponents() { return _components; }
	};
}