#include "PhysicalObject.h"
#include <OgreSceneManager.h>
#include <OgreUserObjectBindings.h>
#include "AnimationController.h"
#include "GameWorld.h"
#include <OgreSubEntity.h>
#include "Player.h"

#define PLAYERCOLOR_PARAMINDEX 1

namespace FlagRTS
{
	PhysicalObject::PhysicalObject(PhysicalObjectDefinition* poDef) :
		SceneObject(static_cast<SceneObjectDefinition*>(poDef)),
		_physicalDef(static_cast<PhysicalObjectDefinition*>(_definition)),
		_animControl(xNew1(AnimationControler,this)),
		_entity(0)
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
	}

	void PhysicalObject::Despawn()
	{
		_sceneNode->detachObject(_entity);
		SceneObject::Despawn();
	}

	void PhysicalObject::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		// Load mesh
		_entity = ogreMgr->createEntity( GetPhysicalObjectDefinition()->GetModelName() );
		//_entity->setQueryFlags( _selectionFlags );
		_entity->setCastShadows(true);
		// Bind this object to ogre entity to get it in scene queries etc
		_entity->getUserObjectBindings().setUserAny(Ogre::Any((SceneObject*)this));
		// Set animations
		auto animations =  GetPhysicalObjectDefinition()->GetAnimations();
		_animControl->SetAnimations(animations);

		SceneObject::LoadResources(ogreMgr);

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
}