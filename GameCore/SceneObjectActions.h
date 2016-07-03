#pragma once

#include "IAction.h"
#include "DataTypes.h"
#include "IObjectRetriever.h"

namespace FlagRTS
{
	class SceneObject;

	class MoveAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*,
		IObjectRetriever<Vector3>*>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;
		IObjectRetriever<Vector3>* _move;

	public:
		MoveAction();
		~MoveAction();

		void Execute();

		void SetParameters(IObjectRetriever<SceneObject*>*, 
			IObjectRetriever<Vector3>*);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};

	class SetPositionAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*,
		IObjectRetriever<Vector3>*>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;
		IObjectRetriever<Vector3>* _position;

	public:
		SetPositionAction();
		~SetPositionAction();

		void Execute();
		void SetParameters(IObjectRetriever<SceneObject*>*, 
			IObjectRetriever<Vector3>*);

		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};

	class SpawnAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*,
		IObjectRetriever<Vector3>*,
		IObjectRetriever<Quaternion>*,
		IObjectRetriever<bool>*>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;
		IObjectRetriever<Vector3>* _position;
		IObjectRetriever<Quaternion>* _rotation;
		IObjectRetriever<bool>* _spawnOnTerrain;

	public:
		SpawnAction();
		~SpawnAction();

		void Execute();

		void SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<Vector3>* posRetriever,
			IObjectRetriever<Quaternion>* rotationRetriever,
			IObjectRetriever<bool>*);
		
		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};

	class DespawnAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*>
	{
		IObjectRetriever<SceneObject*>* _object;
	public:
		DespawnAction();
		~DespawnAction();

		void Execute();

		void SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever);
		
		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};

	class RotateAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*,
		IObjectRetriever<Quaternion>*>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;
		IObjectRetriever<Quaternion>* _rotation;

	public:
		RotateAction();
		~RotateAction();

		void Execute();

		void SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<Quaternion>* rotation);
		
		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};

	class SetOrientationAction : public IParametrizedAction<
		IObjectRetriever<SceneObject*>*,
		IObjectRetriever<Quaternion>*>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;
		IObjectRetriever<Quaternion>* _rotation;

	public:
		SetOrientationAction();
		~SetOrientationAction();

		void Execute();

		void SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<Quaternion>* rotation);
		
		struct Factory : public IActionFactory
		{
			IAction* Create(XmlNode* node);
		};
	};
}