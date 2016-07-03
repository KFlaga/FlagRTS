#pragma once

#include <DataTypes.h>

namespace FlagRTS
{
	class GameCamera;

	// Camera executor handle camera operations :
	// Actions calls its specific funtions 
	// Subclass need to overload camera setter/getter
	// rest direct camera operations are optional - 
	// by default performs operations on camera
	// get by GetActiveCamera
	// Some operations that aren't called directly
	// on camera are optional too, by default just do nothing
	class CameraActionExecutor
	{
	public:
		virtual void SetActiveCamera(GameCamera* camera) = 0;
		virtual GameCamera* GetActiveCamera() const = 0;

		virtual void MoveCamera(const Vector3& vec) = 0;
		virtual void SetCameraPosition(const Vector3& vec) = 0;
		virtual const Vector3& GetCameraPosition() const = 0;

		virtual void RotateCamera(const Quaternion& q) = 0;
		virtual void SetCameraOrientation(const Quaternion& q) = 0;
		virtual const Quaternion& GetCameraOrientation() const = 0;

		virtual void PitchCamera(const Radian& angle) = 0;
		virtual void SetCameraPitch(const Radian& angle) = 0;
		virtual const Radian& GetCameraPitch() const = 0;

		virtual void YawCamera(const Radian& angle) = 0;
		virtual void SetCameraYaw(const Radian& angle) = 0;
		virtual const Radian& GetCameraYaw() const = 0;

		virtual void RollCamera(const Radian& angle) = 0;
		virtual void SetCameraRoll(const Radian& angle) = 0;
		virtual const Radian& GetCameraRoll() const = 0;

		virtual void ZoomCamera(float change) = 0;
		virtual void SetCameraZoom(float level) = 0;
		virtual float GetCameraZoom() const = 0;

		virtual void SetFixedCamera(bool value) = 0;
		virtual void SetMiddlePressTransalte(bool value) = 0;
		virtual void SetMiddlePressRotate(bool value) = 0;
		virtual void SetScrollZoom(bool value) = 0;
		virtual void SetMouseMoveTranslate(bool value) = 0;
		virtual void SetMouseMoveRotate(bool value) = 0;
	};
}