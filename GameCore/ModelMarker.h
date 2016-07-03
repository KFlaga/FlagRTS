#pragma once

#include "SceneMarker.h"
#include "ModelMarkerDefinition.h"
#include <OgreEntity.h>

namespace FlagRTS
{
	// SceneMarker using Ogre::Entity as scene object
	class ModelMarker : public SceneMarker
	{
	public:
		ModelMarker(ModelMarkerDefinition* tpmDef);
		~ModelMarker();

		ModelMarkerDefinition* GetModelMarkerDefinition() const { return static_cast<ModelMarkerDefinition*>(_definition); }

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);
		void Spawn();
		void Despawn();

		Ogre::Entity* GetEntity() const { return static_cast<Ogre::Entity*>(_sceneObject); }
	};
}