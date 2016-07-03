#include "ObjectOutline.h"
#include <OgreSceneManager.h>
#include <OgreUserObjectBindings.h>
#include <OgreMaterialManager.h>

namespace FlagRTS
{
	ObjectOutline::ObjectOutline(PhysicalObjectDefinition* def) :
		PhysicalObject(xNew0(PhysicalObjectDefinition)),
		_isStateValid(true)
	{
		auto pdef = GetPhysicalObjectDefinition();
		pdef->SetFinalType(GetTypeId<ObjectOutline>());
		pdef->SetFootprint(0);
		pdef->SetFinalTypeName("ObjectOutline");
		pdef->SetIsHoverable(false);
		pdef->SetIsIndependent(true);
		pdef->SetIsSelectable(false);
		pdef->SetPathingBlockedGroups(0);
		pdef->SetPathingGroup(0);
		pdef->SetInheritsScale(false);
		pdef->SetModelName( def->GetModelName() );
		pdef->SetModelGroup( def->GetModelGroup() );
		pdef->SetScale( def->GetScale() );
	}

	ObjectOutline::~ObjectOutline()
	{
		xDelete(static_cast<PhysicalObjectDefinition*>(_definition));
	}

	void ObjectOutline::Spawn()
	{
		PhysicalObject::Spawn();
	}

	void ObjectOutline::Despawn()
	{
		PhysicalObject::Despawn();
	}

	void ObjectOutline::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		// Change loading -> omit animations and queries, set custom material

		_entity = ogreMgr->createEntity( GetPhysicalObjectDefinition()->GetModelName() );
		_entity->setCastShadows(false);
		_entity->setMaterial(
			Ogre::MaterialManager::getSingleton().getByName("OutlineGreen", "Base"));

		SceneObject::LoadResources(ogreMgr);
	}

	void ObjectOutline::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		PhysicalObject::UnloadResources(ogreMgr);
	}

	void ObjectOutline::SetIsStateValid(bool value)
	{
		if(value != _isStateValid)
		{
			_isStateValid = value;
			if(value == true)
			{
				_entity->setMaterial(Ogre::MaterialManager::getSingleton().
					getByName("OutlineGreen", "Base"));
			}
			else
			{
				_entity->setMaterial(Ogre::MaterialManager::getSingleton().
					getByName("OutlineRed", "Base"));
			}
		}
	}
}