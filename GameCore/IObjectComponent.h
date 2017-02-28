#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	class IGameObject;
	class ObjectDefinition;

	/// Base class for objects components
	/**
		
		SOME DRAFT NOTES:
		- Object components are 'extensions' to GameObjects classes.
		- They are integral parts of object instances.
		- Adds some functionality to object and required data.
		- Should have little sense to live without object.
		- Each component is supplied to its master object after the object is created.
		- Each frame each object updates its components.
		- Well components also may have object-kind specific functionality, so
			each component defines object instance and object class part.
			Class specific data should be accessible from object definition.
		- Components adds states/commands specific to its functionality.
			
		- Created in object pools together with objects.
		- Components are created via ComponentFactories (much like ObjectDefinitions)
		- In object definition should be stored which named components should object use.
		- Object pools registers pairs (Class, ComponentFactory)
		- Components are GameObject type specific (of course Units can have SceneObject comps)

		- Example components:
			- for SceneObject : Mover, Minimap, Statistics?, Triggers?, Effects?
			- for PhysicalObject : Physics, Pathing, Interaction?
			- for Unit : MoveStrategy, Weapon, Construction, Production, Training, Storage, Experience, Skills
			- for SceneMarker : Interaction?, TimedLife?
			- for FxObject :  
			- for Missle : 
	*/

	class IObjectComponent
	{
		DISALLOW_COPY(IObjectComponent);

	protected:
		IGameObject* _owner;
		ObjectDefinition* _definition;
		ObjectHandle _handle;

	public:
		IObjectComponent(ObjectDefinition* definition, IGameObject* owner) :
			_definition(definition),
			_owner(owner)
		{ 
			_handle.Object = reinterpret_cast<size_t>(this);
		}

		virtual ~IObjectComponent() { }

		virtual void Update(float ms) = 0;
		virtual void LoadResources(Ogre::SceneManager* ogreMgr) = 0;
		virtual void UnloadResources(Ogre::SceneManager* ogreMgr) = 0;
		
		void SetFinalType(TypeId type) { _handle.Type; }
		TypeId GetFinalType() const { return _handle.Type; }

		template<typename OwnerType = IGameObject>
		OwnerType* GetOwner() const { return static_cast<OwnerType*>(_owner); }
		
		template<typename DefinitionType = ObjectDefinition>
		DefinitionType* GetDefinition() const { return static_cast<DefinitionType*>(_definition); }
	};
}