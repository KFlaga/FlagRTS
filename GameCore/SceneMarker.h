#pragma once

#include "SceneObject.h"
#include "SceneMarkerDefinition.h"
#include <OgreMovableObject.h>

namespace FlagRTS
{
	class AnimationControler;

	class SceneMarker : public SceneObject
	{
	protected:
		Ogre::MovableObject* _sceneObject;
		int _owner; // markers are only visible if client == owner
		AnimationControler* _animControl;
		float _timeoutTimer;

	public:
		SceneMarker(SceneMarkerDefinition* smDef);
		~SceneMarker();

		void Update(float ms);

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		SceneMarkerDefinition* GetSceneMarkerDefinition() const
		{
			return static_cast<SceneMarkerDefinition*>(_definition);
		}

		Ogre::MovableObject* GetOgreMovable() const
		{
			return _sceneObject;
		}

		AnimationControler& GetAnimController() { return *_animControl; }
		const AnimationControler& GetAnimController() const { return *_animControl; }

		bool GetIsInteractable() const { return GetSceneMarkerDefinition()->GetIsInteractable(); }
		bool GetIsTimed() const { return GetSceneMarkerDefinition()->GetIsTimed(); }
		float GetTimeout() const { return GetSceneMarkerDefinition()->GetTimeout(); }

		void SetOwner(int owner) { _owner = owner; }
		int GetOwner() const { return _owner; }

		// Returns aabb in world coords
		const Ogre::AxisAlignedBox& GetBoudningBox() const { return _sceneObject->getWorldBoundingBox(); }
		// Returns aabb in local coords ( size of entity )
		Ogre::AxisAlignedBox GetLocalBoundingBox() const
		{
			return Ogre::AxisAlignedBox(
				_sceneObject->getBoundingBox().getMinimum() * GetScale(), 
				_sceneObject->getBoundingBox().getMaximum() * GetScale());
		}

		Vector3 GetHalfSize() const
		{
			return _sceneObject->getBoundingBox().getHalfSize() * GetScale();
		}

		Vector3 GetSize() const
		{
			return _sceneObject->getBoundingBox().getSize() * GetScale();
		}

		// Sets scale of node, so object will have given size
		void SetSize(const Vector3& size)
		{
			Scale( size / (_sceneObject->getBoundingBox().getSize() * GetScale()) );
		}

		// Sets scale of node, so object will have given size
		void SetSizeAbsolute(const Vector3& size)
		{
			Scale( size / _sceneObject->getWorldBoundingBox().getSize() );
		}
	};

	
}