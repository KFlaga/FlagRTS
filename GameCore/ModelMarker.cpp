#include "ModelMarker.h"
#include <OgreSceneManager.h>

namespace FlagRTS
{
	ModelMarker::ModelMarker(ModelMarkerDefinition* def) :
		SceneMarker(def)
	{
		SetFinalType(GetTypeId<ModelMarker>());
	}

	ModelMarker::~ModelMarker()
	{

	}

	void ModelMarker::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			// Load mesh
			_sceneObject = ogreMgr->createEntity( GetModelMarkerDefinition()->GetModelName() );
			_sceneObject->setCastShadows(false);
			_sceneObject->setLightMask(0);

			SceneMarker::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void ModelMarker::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			ogreMgr->destroyEntity(GetEntity());
			_sceneObject = 0;
			_loaded = false;
		}
	}

	void ModelMarker::Spawn()
	{
		if( !_spawned )
		{
			_sceneNode->attachObject(_sceneObject);
			_sceneNode->setScale(GetModelMarkerDefinition()->GetScale());
			SceneMarker::Spawn();
			_spawned = true;
		}
	}

	void ModelMarker::Despawn()
	{
		if( _spawned )
		{
			_sceneNode->detachObject(_sceneObject);
			SceneMarker::Despawn();
			_spawned = false;
		}
	}
}