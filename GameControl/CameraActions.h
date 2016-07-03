#pragma once

#include <IAction.h>
#include "CameraActionExecutor.h"
#include <OgreVector3.h>
#include <OgreQuaternion.h>

namespace ORTS
{
	// Additional step in inheritance for action family recognition
	class CameraAction : public IAction_old
	{
	public:
		CameraAction() : IAction_old() { }

		TypeId GetActionFamily() const { return GetTypeId<CameraAction>(); }
	};

	class CameraMoveAction : public CameraAction
	{
	private:
		Vector3 _translation;

	public:
		CameraMoveAction() : CameraAction() { }

		const Vector3& GetTranslation() const { return _translation; }
		void SetTranslation(const Vector3& trans) { _translation = trans; }

		void DoAction()
		{
			_executor.CastRaw<CameraActionExecutor>()->MoveCamera(_translation);
		}

		TypeId GetActionType() const { return GetTypeId<CameraMoveAction>(); }
	};

	class CameraSetPositionAction : public CameraAction
	{
	private:
		Vector3 _position;

	public:
		CameraSetPositionAction() : CameraAction() { }

		const Vector3& GetPosition() const { return _position; }
		void SetPosition(const Vector3& pos) { _position = pos; }

		void DoAction()
		{
			_executor.CastRaw<CameraActionExecutor>()->SetCameraPosition(_position);
		}

		TypeId GetActionType() const { return GetTypeId<CameraSetPositionAction>(); }
	};

	class CameraRotateAction : public CameraAction
	{
	private:
		Quaternion _rotation;

	public:
		CameraRotateAction() : CameraAction() { }

		const Quaternion& GetRotation() const { return _rotation; }
		void SetRotation(const Quaternion& rot) { _rotation = rot; }

		void DoAction()
		{
			_executor.CastRaw<CameraActionExecutor>()->RotateCamera(_rotation);
		}

		TypeId GetActionType() const { return GetTypeId<CameraRotateAction>(); }
	};

	class CameraSetOrientationAction : public CameraAction
	{
	private:
		Quaternion _orientation;

	public:
		CameraSetOrientationAction() : CameraAction() { }

		const Quaternion& GetOrientation() const { return _orientation; }
		void SetOrientation(const Quaternion& q) { _orientation = q; }

		void DoAction()
		{
			_executor.CastRaw<CameraActionExecutor>()->SetCameraOrientation(_orientation);
		}

		TypeId GetActionType() const { return GetTypeId<CameraSetOrientationAction>(); }
	};

	class CameraSetActiveAction : public CameraAction
	{
	private:
		RefPtr<GameCamera> _camera;

	public:
		CameraSetActiveAction() : CameraAction() { }

		const RefPtr<GameCamera>& GetOrientation() const { return _camera; }
		void SetOrientation(const RefPtr<GameCamera>& cam) { _camera = cam; }

		void DoAction()
		{
			_executor.CastRaw<CameraActionExecutor>()->SetActiveCamera(_camera);
		}

		TypeId GetActionType() const { return GetTypeId<CameraSetActiveAction>(); }
	};
}