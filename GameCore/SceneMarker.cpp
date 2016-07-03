#include "SceneMarker.h"
#include <OgreSceneManager.h>
#include "AnimationController.h"

namespace FlagRTS
{
	SceneMarker::SceneMarker(SceneMarkerDefinition* def) :
		SceneObject(static_cast<SceneObjectDefinition*>(def)),
		_animControl(xNew1(AnimationControler,this)),
		_timeoutTimer(def->GetTimeout())
	{

	}

	SceneMarker::~SceneMarker()
	{
		xDelete(_animControl);
	}

	void SceneMarker::Update(float ms)
	{

	}

	void SceneMarker::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		auto animations =  GetSceneMarkerDefinition()->GetAnimations();
		_animControl->SetAnimations(animations);

		// Bind this object to ogre movable
		_sceneObject->getUserObjectBindings().setUserAny(Ogre::Any((SceneObject*)this));
		_sceneObject->setQueryFlags(0);
	}

	void SceneMarker::UnloadResources(Ogre::SceneManager* ogreMgr)
	{

	}
}