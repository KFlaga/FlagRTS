#pragma once

#include "GameCamera.h"
#include <Event.h>

namespace FlagRTS
{
	// Camera controller changes active camera
	// Camera specific manipulations are performed on active camera
	// Also responsible for updating mouse real position ( on camera plane ) and cast ray
	class CameraController
	{
	private:
		GameCamera* _activeCamera;
		Ogre::SceneManager* _ogreMgr;
		Ogre::RenderWindow* _window;
		uint8 _viewportNum;
		bool _isRendering;

		bool _isFixed; // If true camera cannot be moved
		bool _isMiddlePressTranslating; // If middle is pressed, then mosur move translates camera
		bool _isMiddlePressRotating; // If middle is pressed, then mosur move rotates camera
		bool _isScrollZooming; // If true scrolling with mouse zomms camera
		bool _isMouseMoveTransating; // If true moving cursor to screen bound moves camera
		bool _isMouseMoveRotating; // If true moving mouse rotates camera

		Event<GameCamera*> _cameraChanged;

	public:
		CameraController(Ogre::SceneManager* ogreMgr, 
			Ogre::RenderWindow* window, uint8 cameraViewport);
		~CameraController();

		// Changes active camera. If camera is rendering, then recreates
		// window viewport
		void SetActiveCamera(GameCamera* camera);
		GameCamera* GetActiveCamera() const { return _activeCamera; }

		// Creates viewport on Zindex -1 from active camera, so 
		// what camera sees is rendered
		void BeginCameraRender();
		// Removes camera viewport, so game is no longer rendered
		void EndCameraRender();

		// Checks input and moves/rotates camera accordlingly
		void Update(float ms);

		// Returns ray from given screen point ( in range [0-1] ) in camera direction
		Ogre::Ray GetCastingRay(const Vector2& screenPos) const
		{
			return _activeCamera->GetOgreCamera()->getCameraToViewportRay(
				screenPos.x, screenPos.y);
		}

		Event<GameCamera*>& ActiveCameraChanged() { return _cameraChanged; }

		void MoveCamera(const Vector3& vec)
		{
			GetActiveCamera()->Move(Quaternion(GetActiveCamera()->GetYaw(), Vector3(0.f,1.f,0.f)) * vec);
		}

		void SetCameraPosition(const Vector3& vec)
		{
			GetActiveCamera()->SetPosition(vec);
		}

		const Vector3& GetCameraPosition() const
		{
			return GetActiveCamera()->GetPosition();
		}

		void RotateCamera(const Quaternion& q)
		{
			GetActiveCamera()->Rotate(q);
		}

		void SetCameraOrientation(const Quaternion& q)
		{
			GetActiveCamera()->SetOrientation(q);
		}

		const Quaternion& GetCameraOrientation() const
		{
			return GetActiveCamera()->GetOrientation();
		}

		void PitchCamera(const Radian& angle)
		{
			GetActiveCamera()->Pitch(angle);
		}

		void SetCameraPitch(const Radian& angle)
		{
			GetActiveCamera()->SetPitch(angle);
		}

		const Radian& GetCameraPitch() const
		{
			return GetActiveCamera()->GetPitch();
		}

		void YawCamera(const Radian& angle)
		{
			GetActiveCamera()->Yaw(angle);
		}

		void SetCameraYaw(const Radian& angle)
		{
			GetActiveCamera()->SetYaw(angle);
		}

		const Radian& GetCameraYaw() const
		{
			return GetActiveCamera()->GetYaw();
		}

		void RollCamera(const Radian& angle)
		{
			GetActiveCamera()->Roll(angle);
		}

		void SetCameraRoll(const Radian& angle)
		{
			GetActiveCamera()->SetRoll(angle);
		}

		const Radian& GetCameraRoll() const
		{
			return GetActiveCamera()->GetRoll();
		}

		void ZoomCamera(float change)
		{
			GetActiveCamera()->Zoom(change);
		}

		void SetCameraZoom(float level)
		{
			GetActiveCamera()->SetZoomValue(level);
		}

		float GetCameraZoom() const
		{
			return GetActiveCamera()->GetZoomValue();
		}

		void SetFixedCamera(bool value) { }
		void SetMiddlePressTransalte(bool value) {}
		void SetMiddlePressRotate(bool value) {}
		void SetScrollZoom(bool value) {}
		void SetMouseMoveTranslate(bool value){}
		void SetMouseMoveRotate(bool value){}
	};
}