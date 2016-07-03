#pragma once

#include "SceneObjectDefinition.h"
#include "DataTypes.h"
#include <Array.h>

namespace FlagRTS
{
	// Camera zoom levels defines relation of 
	// precentage of zoom and camera pitch / z position ( relative to ground )
	// Between levels there's linera interpolation
	class CameraZoomLevel
	{
	public:
		CameraZoomLevel() { }

		CameraZoomLevel(float zoom, Radian pitch, float z) :
			Level(zoom), Pitch(pitch), ZDist(z)
		{ }

		float Level;
		Radian Pitch;
		float ZDist;
	};

	// Contains info on initial camera parameters and max bounds
	// ( so camera can be defined to disallow movement etc )
	// Cameras don't use spawn info for orientation but
	//  direction is set when spawning
	class GameCameraDefinition : public SceneObjectDefinition
	{
	protected:
		// Definition of camera frustrum dimensions
		float _nearClip;
		float _farClip;
		Radian _fovY;
		Array<CameraZoomLevel> _zoomLevels;

		bool _useBoundingBox; // If true use rect to limit camera visibility rather than fixed position/direction
							  // Direction must be specified by direction then
		Vector4 _cameraBounds; // Rectangle (top,left,bot,right) which limits camera visibility

		bool _useDirection; // If true uses direction vector to set/limit camera rotation
		Vector3 _direction; // Normalized direction of camera
		Vector3 _maxDirection; // Defines max angles camera can achieve
		Vector3 _minDirection;

		bool _useYawPitch; // If true uses yaw/pitch values rather than directon vector

		Radian _yaw;
		Radian _maxYaw;
		Radian _minYaw;
		bool _useGlobalPitch; // If true yaw rotates around global Y axis, oherwise local one

		Radian _pitch;
		Radian _maxPitch;
		Radian _minPitch;

	public:
		// Sets default values
		GameCameraDefinition();
		// Reads values from xml node
		GameCameraDefinition(XmlNode* soDefNode);

		Radian GetFovY() const { return _fovY; }
		float GetNearClip() const { return _nearClip; }
		float GetFarClip() const { return _farClip; }

		Vector3 GetDirection() const { return _direction; }
		void SetDirection(const Vector3& dir) { _direction = dir; }

		Vector3 GetMaxDirection() const { return _maxDirection; }
		void SetMaxDirection(const Vector3& dir) { _maxDirection = dir; }

		Vector3 GetMinDirection() const { return _minDirection; }
		void SetMinDirection(const Vector3& dir) { _minDirection = dir; }

		bool GetUseDirection() const { return _useDirection; }
		void SetUseDirection(bool useDir) { _useDirection = useDir; }

		bool GetUseBoundingBox() const { return _useBoundingBox; }
		void SetUseBoundingBox(bool useBB) { _useBoundingBox = useBB; }

		bool GetUseYawPitch() const { return _useYawPitch; }
		void SetUseYawPitch(bool useYP) { _useYawPitch = useYP; }

		Vector4 GetBoundingBox() const { return _cameraBounds; }
		void SetBoundingBox(const Vector4& bb) { _cameraBounds = bb; }

		const Array<CameraZoomLevel>& GetZoomLevels() const { return _zoomLevels; }
		Array<CameraZoomLevel>& GetZoomLevelsModify() { return _zoomLevels; }
	};
}

