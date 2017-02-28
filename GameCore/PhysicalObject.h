#pragma once

#include "SceneObject.h"
#include "PhysicalObjectDefinition.h"
#include <OgreEntity.h>

namespace FlagRTS
{
	class AnimationControler;

	// PhysicalObjects are scene objects that have physical representation and
	// are subjects to world physics ( collisions etc )
	// Uses Ogre::Entity for actual model
	class PhysicalObject : public SceneObject
	{
	protected:
		PhysicalObjectDefinition* _physicalDef;
		Ogre::Entity* _entity;
		AnimationControler* _animControl;

	public:
		PhysicalObject(PhysicalObjectDefinition* poDef );
		virtual ~PhysicalObject();

		PhysicalObjectDefinition* GetPhysicalObjectDefinition() const
		{ 
			return _physicalDef; 
		}

		Ogre::Entity* GetEntity() const { return _entity; }

		AnimationControler& GetAnimController() { return *_animControl; }
		const AnimationControler& GetAnimController() const { return *_animControl; }

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Spawn();
		void Despawn();

		void SetOwner(unsigned int owner);
		
		/**
		Returns AABB in world coords
		*/
		const Ogre::AxisAlignedBox& GetBoudningBox() const 
		{ 
			return _entity->getWorldBoundingBox(); 
		}
		
		/**
		Returns AABB in local coords ( size of entity )
		*/
		Ogre::AxisAlignedBox GetLocalBoundingBox() const
		{
			return Ogre::AxisAlignedBox(
				_entity->getBoundingBox().getMinimum() * _physicalDef->GetScale(), 
				_entity->getBoundingBox().getMaximum() * _physicalDef->GetScale());
		}

		Vector3  GetHalfSize() const
		{
			return _entity->getBoundingBox().getHalfSize() * _physicalDef->GetScale();
		}

		Vector3 GetSize() const
		{
			return _entity->getBoundingBox().getSize() * _physicalDef->GetScale();
		}
		
		/**
		Sets scale of node, so that model will have given size
		*/
		void SetSize(const Vector3& size)
		{
			Scale( size / (_entity->getBoundingBox().getSize() * GetScale()) );
		}

		const string& GetPlayerColorMaterial() const { return _physicalDef->GetPlayerColorMaterial(); }
	};
}