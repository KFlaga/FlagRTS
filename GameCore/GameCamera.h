#pragma once

#include "DataTypes.h"
#include "SceneObject.h"
#include "GameCameraDefinition.h"
#include <OgreCamera.h>

namespace FlagRTS
{
	// Cameras are used to create viewports for game screen
	// Camera is moved via its scene node movement ( using functions from SceneObject )
	// Cameras are changed via CameraController ( only one gamecamera can be active )
	class GameCamera : public SceneObject
	{
	private:
		GameCameraDefinition* _cameraDef;
		Ogre::Camera* _ogreCamera;
		Ogre::Viewport* _camViewport;
		Vector3 _camDirection; // Direction stored also here, as it yaw/pitch will be converted to this
		float _zoom;

	public:
		// Creates Ogre Camera from given data
		GameCamera(GameCameraDefinition* _cameraDefinition);

		// Creates new viewport for given window with internal camera
		// Must be called after LoadResources()
		// Size of viewport can be defined by rect : left,top,width,height
		void AttachCameraToWindow(Ogre::RenderWindow* window, uint8 viewportNum, const Vector4& rect = Vector4(0.0f,0.0f,1.0f,1.0f));
		void DetachCameraFromWindow(Ogre::RenderWindow* window);

		// Changes size of viewport camera is rendering to rect = left,top,width,height
		void SetViewportDimensions(const Vector4& rect = Vector4(0.0f,0.0f,1.0f,1.0f));

		// Attaches camera to camera node attached to root scene node
		void Spawn();
		// Detaches camera from camera node
		void Despawn();

		// Camera does nothing on updates for now
		// ( may add states like follow, smooth zoom etc )
		void Update(float ms);

		// Creates camera and sets its parameters
		void LoadResources(Ogre::SceneManager* ogreMgr);
		// Deletes ogre camera ( DetachFromWindow should be called before )
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		GameCameraDefinition* GetGameCameraDefinition() const
		{ 
			return _cameraDef; 
		}

		Ogre::Camera* GetOgreCamera() const { return _ogreCamera; }
		Ogre::Viewport* GetCameraViewport() const { return _camViewport; }

		float GetFocalLength() const { return _ogreCamera->getFocalLength(); }
		void SetFocalLength(float fl) { _ogreCamera->setFocalLength(fl); }

		Radian GetFieldOfViewY() const { return _ogreCamera->getFOVy(); }
		void SetFieldOfViewY(const Radian& fov) { _ogreCamera->setFOVy(fov); }

		void SetNearClipDist(float dist) { _ogreCamera->setNearClipDistance(dist); }
		float GetNearClipDist() const { _ogreCamera->getNearClipDistance(); }

		void SetFarClipDist(float dist) { _ogreCamera->setFarClipDistance(dist); }
		float GetFarClipDist() const { _ogreCamera->getFarClipDistance(); }

		bool GetUseDirection() const { return GetGameCameraDefinition()->GetUseDirection(); }
		bool GetUseBoundingBox() const { return GetGameCameraDefinition()->GetUseBoundingBox(); }
		Vector3 GetMaxDirection() const { return GetGameCameraDefinition()->GetMaxDirection(); }
		Vector3 GetMinDirection() const { return GetGameCameraDefinition()->GetMinDirection(); }

		// Return value of zoom ( from range [0-1] )
		float GetZoomValue() const { return _zoom; }
		// Modifies direction and field of view according to given zoom level
		void SetZoomValue(float zoom);
		// Modifies direction and field of view to zoom by given value
		void Zoom(float zoom)
		{
			float nzoom = _zoom + zoom;
			nzoom = nzoom > 0.0f ? nzoom : 0.0f;
			nzoom = nzoom < 1.0f ? nzoom : 1.0f;
			SetZoomValue(nzoom);
		}

		const Array<CameraZoomLevel>& GetZoomLevels() const { return GetGameCameraDefinition()->GetZoomLevels(); }

		// Derive pitch value from scene node direction ( rotation around local X )
		Radian GetPitch() const;
		// Modify SceneNode direction value to set given pitch
		void SetPitch(Radian angle);
		// Modify SceneNode direction value to change pitch by given value
		void Pitch(Radian angle);
		
		// Derive yaw value from scene node direction ( rotation around global/local Y (depends on flag) )
		Radian GetYaw() const;
		// Modify SceneNode direction value to set given yaw
		void SetYaw(Radian angle);
		// Modify SceneNode direction value to change yaw by given value
		void Yaw(Radian angle);

		// Derive roll value from scene node direction ( rotation around local Z )
		Radian GetRoll() const;
		// Modify SceneNode direction value to set given roll
		void SetRoll(Radian angle);
		// Modify SceneNode direction value to change roll by given value
		void Roll(Radian angle);

		// bool GetMaxYaw() const { return GetGameCameraDefPtr()->GetUseDirection(); }

		void SetGlobalYaw(bool value) { _ogreCamera->setFixedYawAxis(value); }
	};
}