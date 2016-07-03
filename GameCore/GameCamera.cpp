#include "GameCamera.h"
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>
#include "SceneObjectStates.h"
#include "GameWorld.h"
#include "Map.h"

namespace FlagRTS
{
	GameCamera::GameCamera(GameCameraDefinition* cameraDefinition) :
		SceneObject(static_cast<SceneObjectDefinition*>(cameraDefinition)),
		_ogreCamera(0),
		_camViewport(0),
		_cameraDef(static_cast<GameCameraDefinition*>(_definition)),
		_zoom(0.f)
	{
		SetFinalType(GetTypeId<GameCamera>());
	}

	void GameCamera::Update(float ms)
	{

	}

	void GameCamera::AttachCameraToWindow(Ogre::RenderWindow* window, 
		uint8 viewportNum, const Vector4& rect)
	{
		_camViewport = window->addViewport(_ogreCamera, viewportNum, rect.x, rect.y, rect.z, rect.w);
		_camViewport->setBackgroundColour(Ogre::ColourValue(0.1f,0.1f,0.1f,1.0f));
		_ogreCamera->setAspectRatio(1.3333f);

		_camViewport->setOverlaysEnabled(true);
	}

	void GameCamera::DetachCameraFromWindow(Ogre::RenderWindow* window)
	{
		if(_camViewport != 0)
			window->removeViewport(_camViewport->getZOrder());
	}

	void GameCamera::SetViewportDimensions(const Vector4& rect)
	{
		_camViewport->setDimensions(rect.x, rect.y, rect.z, rect.w);
	}

	void GameCamera::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			// Try to find camera if such exists. If not exception is thrown
			// so new camera is created. Otherwise some other name must be choosen
			try
			{
				ogreMgr->getCamera(_definition->GetName());

			}
			catch(...)
			{
				_ogreCamera = ogreMgr->createCamera(_definition->GetName());
			}
			// Set Ogre camera parameters
			auto gcDef = GetGameCameraDefinition();
			_ogreCamera->setFOVy(gcDef->GetFovY());
			_ogreCamera->setNearClipDistance(gcDef->GetNearClip());
			_ogreCamera->setFarClipDistance(gcDef->GetFarClip());
			_ogreCamera->setPosition(Vector3(0.0f,0.0f,0.0f));
			_ogreCamera->setDirection(Vector3(0.0f,0.0f,-1.0f));
			_ogreCamera->setQueryFlags(0);
			_ogreCamera->Ogre::MovableObject::setUserAny(Ogre::Any((SceneObject*)this));

			SceneObject::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void GameCamera::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			ogreMgr->destroyCamera(_ogreCamera);
			SceneObject::UnloadResources(ogreMgr);
			_loaded = false;
		}
	}

	void GameCamera::Spawn()
	{
		if(!_spawned)
		{
			_sceneNode->attachObject(_ogreCamera);
			_stateMachine.ChangeState(SceneObjectStates::Idle);
			SetZoomValue(0.5f);
			SceneObject::Spawn();
			_spawned = true;
		}
	}

	void GameCamera::Despawn()
	{
		if(_spawned)
		{
			_sceneNode->detachObject(_ogreCamera);
			_stateMachine.ChangeState(SceneObjectStates::NotSpawned);
			SceneObject::Despawn();
			_spawned = false;
		}
	}

	Radian GameCamera::GetPitch() const
	{
		return GetOrientation().getPitch(true);
	}

	// Modify SceneNode direction value to set given pitch
	void GameCamera::SetPitch(Radian angle)
	{
		Pitch(angle - GetOrientation().getPitch(true));
	}
	// Modify SceneNode direction value to change pitch by given value
	void GameCamera::Pitch(Radian angle)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(angle), Vector3(1.0f, 0.0f, 0.0f));
		Rotate(q);
	}

	// Derive yaw value from scene node direction
	Radian GameCamera::GetYaw() const
	{
		return GetOrientation().getYaw(true);
	}

	// Modify SceneNode direction value to set given yaw
	void GameCamera::SetYaw(Radian angle)
	{
		Yaw(angle - GetOrientation().getYaw(true));
	}

	// Modify SceneNode direction value to change yaw by given value
	void GameCamera::Yaw(Radian angle)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(angle), Vector3(0.0f, 1.0f, 0.0f));
		Rotate(q);
	}

	// Derive roll value from scene node direction
	Radian GameCamera::GetRoll() const
	{
		return GetOrientation().getRoll(true);
	}

	// Modify SceneNode direction value to set given roll
	void GameCamera::SetRoll(Radian angle)
	{
		Roll(angle - GetOrientation().getRoll(true));
	}

	// Modify SceneNode direction value to change roll by given value
	void GameCamera::Roll(Radian angle)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(angle), Vector3(0.0f, 0.0f, 1.0f));
		Rotate(q);
	}

	void GameCamera::SetZoomValue(float zoom)
	{
		for(auto lvlIt = GetZoomLevels().begin();
			lvlIt != GetZoomLevels().end(); lvlIt++)
		{
			if(lvlIt->Level >= zoom) // this is the higher value zoom / lvlIt-- is lower value
			{
				CameraZoomLevel highZoom = *lvlIt;
				CameraZoomLevel lowZoom = *(--lvlIt);

				float lvlDiff = highZoom.Level - lowZoom.Level;
				float actDiff = zoom - lowZoom.Level;
				float ratio = actDiff/lvlDiff;

				Radian pitch = lowZoom.Pitch + (highZoom.Pitch - lowZoom.Pitch)*ratio;
				float zdist = lowZoom.ZDist + (highZoom.ZDist - lowZoom.ZDist)*ratio;

				auto testPitch = GetPitch();

				SetPitch(pitch);
				const Vector3& camPos = GetPositionAbsolute();
				SetPosition(camPos.x, GameWorld::GlobalWorld->GetMap()->
					GetTerrainHeight(camPos.x, camPos.z) + zdist, camPos.z);

				_zoom = zoom;

				break;
			}
		}
	}
}