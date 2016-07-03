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
		Vector2 _orientedBounds[4]; // 2D - OOBB centered on (0,0) : corners (left-top,rigth-top,left-bot,right-bot)
		size_t _pathingHandle; // Handle to object repsonsible for path finding ( obtained via PathingSystem )
		//CollisionArea _collisionArea; // Shape used for collision checking

		bool _isSelectable;
		bool _isHoverable;

	public:
		PhysicalObject(PhysicalObjectDefinition* poDef );
		virtual ~PhysicalObject();

		PhysicalObjectDefinition* GetPhysicalObjectDefinition() const
		{ 
			return _physicalDef; 
		}

		Ogre::Entity* GetEntity() const { return _entity; }

		size_t GetPathingHandle() const { return _pathingHandle; }
		void SetPathingHandle(const size_t handle) { _pathingHandle = handle; }

		// Returns aabb in world coords
		const Ogre::AxisAlignedBox& GetBoudningBox() const { return _entity->getWorldBoundingBox(); }
		// Returns aabb in local coords ( size of entity )
		Ogre::AxisAlignedBox GetLocalBoundingBox() const
		{
			return Ogre::AxisAlignedBox(_entity->getBoundingBox().getMinimum() * _physicalDef->GetScale(), 
				_entity->getBoundingBox().getMaximum() * _physicalDef->GetScale());
		}

		Vector3 GetHalfSize() const
		{
			return _entity->getBoundingBox().getHalfSize() * _physicalDef->GetScale();
		}

		Vector3 GetSize() const
		{
			return _entity->getBoundingBox().getSize() * _physicalDef->GetScale();
		}

		// Sets scale of node, so model have given size
		void SetSize(const Vector3& size)
		{
			Scale( size / (_entity->getBoundingBox().getSize() * GetScale()) );
		}

		// Returns 2d oobb centered on (0,0)
		const Vector2* GetLocalOrientedBoundingBox() const
		{
			return _orientedBounds;
		}

		AnimationControler& GetAnimController() { return *_animControl; }
		const AnimationControler& GetAnimController() const { return *_animControl; }

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Spawn();
		void Despawn();

		void SetOwner(unsigned int owner);

		bool IsHoverable() const { return _isHoverable; }
		void SetIsHoverable(bool value);

		bool IsSelectable() const { return _isSelectable; }
		void SetIsSelectable(bool value);

		void SetSelectionFlags(uint32 flags);

		short GetPathingGroup() const { return _physicalDef->GetPathingGroup(); }
		short GetPathingBlockedGroups() const { return _physicalDef->GetPathingBlockedGroups(); }
		PathFinding::UniformGridObstacle* GetFootprint() const { return _physicalDef->GetFootprint(); }

		bool GetAddToPathingGraph() const { return _physicalDef->GetAddToPathingGraph(); }

		float GetDistanceToOtherObject(PhysicalObject* target);

		const string& GetPlayerColorMaterial() const { return _physicalDef->GetPlayerColorMaterial(); }

	protected:
		void UpdateBoundingBox(SceneObject* thisObject);
		DEFINE_DELEGATE1(ObjectRotatedDelegate, PhysicalObject,
			UpdateBoundingBox, SceneObject*);
		ObjectRotatedDelegate _updateBoundingBox;

	};
}