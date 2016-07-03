#include "PhysicalObject.h"
#include <OgreSceneManager.h>
#include <OgreUserObjectBindings.h>
#include "AnimationController.h"
#include "SelectionFlags.h"
#include "GameWorld.h"
#include "PathingSystem.h"
#include <OgreSubEntity.h>
#include "Player.h"

#define PLAYERCOLOR_PARAMINDEX 1

namespace FlagRTS
{
	PhysicalObject::PhysicalObject(PhysicalObjectDefinition* poDef) :
		SceneObject(static_cast<SceneObjectDefinition*>(poDef)),
		_physicalDef(static_cast<PhysicalObjectDefinition*>(_definition)),
		_animControl(xNew1(AnimationControler,this)),
		_isSelectable(poDef->GetIsSelectable()),
		_isHoverable(poDef->GetIsHoverable()),
		_updateBoundingBox(this),
		_entity(0),
		_pathingHandle(0)
	{

	}

	PhysicalObject::~PhysicalObject()
	{
		xDelete(_animControl);
	}

	void PhysicalObject::Spawn()
	{
		_sceneNode->attachObject(_entity);
		_sceneNode->setScale(_physicalDef->GetScale());
		SceneObject::Spawn();
		_onRotated += &_updateBoundingBox;
	}

	void PhysicalObject::Despawn()
	{
		_onRotated -= &_updateBoundingBox;
		_sceneNode->detachObject(_entity);
		SceneObject::Despawn();
	}

	void PhysicalObject::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		// Load mesh
		_entity = ogreMgr->createEntity( GetPhysicalObjectDefinition()->GetModelName() );
		_entity->setQueryFlags( _selectionFlags );
		_entity->setCastShadows(true);
		// Bind this object to ogre entity to get it in scene queries etc
		_entity->getUserObjectBindings().setUserAny(Ogre::Any((SceneObject*)this));
		// Set animations
		auto animations =  GetPhysicalObjectDefinition()->GetAnimations();
		_animControl->SetAnimations(animations);

		SceneObject::LoadResources(ogreMgr);

		Vector3 halfSize(GetHalfSize());

		_orientedBounds[0] = Vector2(-halfSize.x, -halfSize.z);
		_orientedBounds[1] = Vector2(halfSize.x, -halfSize.z);
		_orientedBounds[2] = Vector2(-halfSize.x, halfSize.z);
		_orientedBounds[3] = Vector2(halfSize.x, halfSize.z);

		if( GetPlayerColorMaterial().size() > 0)
		{
			// Find sub-entity with this material
			for(unsigned int i = 0; i < _entity->getNumSubEntities(); ++i)
			{
				auto subEntity = _entity->getSubEntity(i);
				if(subEntity->getMaterialName() == GetPlayerColorMaterial())
				{
					const Vector3& color = GameWorld::GlobalWorld->GetPlayers()->
						GetPlayer(_owner)->GetColor();
					subEntity->setCustomParameter(PLAYERCOLOR_PARAMINDEX, 
						Vector4(color.x, color.y, color.z, 1.f));
					break;
				}
			}
		}
	}

	void PhysicalObject::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		ogreMgr->destroyEntity(_entity);
		_entity = 0;
		SceneObject::UnloadResources(ogreMgr);
	}

	void PhysicalObject::SetOwner(unsigned int owner)
	{
		SceneObject::SetOwner(owner);
		if( GetPlayerColorMaterial().size() > 0 && _entity != 0)
		{
			// Find sub-entity with this material
			for(unsigned int i = 0; i < _entity->getNumSubEntities(); ++i)
			{
				auto subEntity = _entity->getSubEntity(i);
				if(subEntity->getMaterialName() == GetPlayerColorMaterial())
				{
					const Vector3& color = GameWorld::GlobalWorld->GetPlayers()->GetPlayer(_owner)->GetColor();
					subEntity->setCustomParameter(PLAYERCOLOR_PARAMINDEX, 
						Vector4(color.x, color.y, color.z, 1.f));
					break;
				}
			}
		}
	}

	void PhysicalObject::SetSelectionFlags(uint32 flags)
	{
		_selectionFlags = flags;
		if(_entity != 0)
		{
			_entity->setQueryFlags(flags);
		}
	}

	void PhysicalObject::SetIsHoverable(bool value)
	{
		_isHoverable = value;
		value == true ?
			AddSelectionFlags(SelectionFlags::Hoverable) :
			RemoveSelectionFlags(SelectionFlags::Hoverable);
	}

	void PhysicalObject::SetIsSelectable(bool value)
	{
		_isSelectable = value;
		value == true ?
			AddSelectionFlags(SelectionFlags::Selectable) :
			RemoveSelectionFlags(SelectionFlags::Selectable);
	}

	void PhysicalObject::UpdateBoundingBox(SceneObject* )
	{
		Vector3 halfSize(GetOrientationAbsolute() * GetHalfSize());

		_orientedBounds[0] = Vector2(-halfSize.x, -halfSize.z);
		_orientedBounds[1] = Vector2(halfSize.x, -halfSize.z);
		_orientedBounds[2] = Vector2(-halfSize.x, halfSize.z);
		_orientedBounds[3] = Vector2(halfSize.x, halfSize.z);
	}

	float PhysicalObject::GetDistanceToOtherObject(PhysicalObject* target)
	{
		return GameWorld::GlobalWorld->GetPathingSystem()->GetDistanceBetweenObjects(this, target);
	}
}