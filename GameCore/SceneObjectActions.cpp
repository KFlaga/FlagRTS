#include "SceneObjectActions.h"
#include "ISceneObjectRetriever.h"
#include "Vector2Retrievers.h"
#include "Vector3Retrievers.h"
#include "QuaternionRetrievers.h"
#include "OwnerRetrievers.h"
#include "BoolRetrievers.h"
#include "SceneObject.h"

#include "GameWorld.h"

namespace FlagRTS
{
	MoveAction::MoveAction() :
		_object(0),
		_move(0)
	{ }

	MoveAction::~MoveAction()
	{
		xDeleteSafe(_object);
		xDeleteSafe(_move);
	}

	void MoveAction::Execute()
	{
		(static_cast<SceneObject*>(*_object))->Move(*_move);
	}

	void MoveAction::SetParameters(
		IObjectRetriever<SceneObject*>* object, 
			IObjectRetriever<Vector3>* move)
	{
		_object = object;
		_move = move;
	}

	IAction* MoveAction::Factory::Create(XmlNode* actionNode)
	{
		MoveAction* newAction(xNew0(MoveAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));
		auto move = Vector3RetrieverFactory::Create(
			actionNode->first_node("Move"));

		newAction->SetParameters(object, move);
		return newAction;
	}

	SetPositionAction::SetPositionAction() :
		_object(0),
		_position(0)
	{ }

	SetPositionAction::~SetPositionAction()
	{
		xDeleteSafe(_object);
		xDeleteSafe(_position);
	}

	void SetPositionAction::Execute()
	{
		(static_cast<SceneObject*>(*_object))->SetPosition(*_position);
	}

	void SetPositionAction::SetParameters(
		IObjectRetriever<SceneObject*>* object, 
			IObjectRetriever<Vector3>* move)
	{
		_object = object;
		_position = move;
	}

	IAction* SetPositionAction::Factory::Create(XmlNode* actionNode)
	{
		SetPositionAction* newAction(xNew0(SetPositionAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));
		auto move = Vector3RetrieverFactory::Create(
			actionNode->first_node("Position"));

		newAction->SetParameters(object, move);
		return newAction;
	}

	SpawnAction::SpawnAction() :
		_object(0),
		_position(0),
		_rotation(0)
	{ }

	SpawnAction::~SpawnAction()
	{
		xDeleteSafe(_object);
		xDeleteSafe(_position);
		xDeleteSafe(_rotation);
	}

	void SpawnAction::Execute()
	{
		GameWorld::GlobalWorld->SpawnSceneObject(
			*_object, 
			SpawnInfo(*_rotation , *_position, *_spawnOnTerrain));
	}

	void SpawnAction::SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<Vector3>* posRetriever,
			IObjectRetriever<Quaternion>* roationRetriever,
			IObjectRetriever<bool>* spawnTerrain)
	{
		_object = soRetriever;
		_position = posRetriever;
		_rotation = roationRetriever;
		_spawnOnTerrain = spawnTerrain;
	}

	IAction* SpawnAction::Factory::Create(XmlNode* actionNode)
	{
		SpawnAction* newAction(xNew0(SpawnAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));
		auto move = Vector3RetrieverFactory::Create(
			actionNode->first_node("Position"));
		auto rotation = QuaternionRetrieverFactory::Create(
			actionNode->first_node("Orientation"));
		auto spawnOnTerrain = BoolRetrieverFactory::Create(
			actionNode->first_node("SpawnOnTerrain"));

		newAction->SetParameters(object, move, rotation, spawnOnTerrain);
		return newAction;
	}

	DespawnAction::DespawnAction() :
		_object(0)
	{

	}

	DespawnAction::~DespawnAction()
	{
		xDeleteSafe(_object);
	}

	void DespawnAction::Execute()
	{
		GameWorld::GlobalWorld->DespawnSceneObject(*_object);
	}

	void DespawnAction::SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever)
	{
		_object = soRetriever;
	}

	IAction* DespawnAction::Factory::Create(XmlNode* actionNode)
	{
		DespawnAction* newAction(xNew0(DespawnAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));

		newAction->SetParameters(object);
		return newAction;
	}

	RotateAction::RotateAction() :
		_object(0),
		_rotation(0)
	{ }

	RotateAction::~RotateAction()
	{
		xDeleteSafe(_object);
		xDeleteSafe(_rotation);
	}

	void RotateAction::Execute()
	{
		(static_cast<SceneObject*>(*_object))->Rotate(*_rotation);
	}

	void RotateAction::SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<Quaternion>* rotationRetriever)
	{
		_object = soRetriever;
		_rotation = rotationRetriever;
	}

	IAction* RotateAction::Factory::Create(XmlNode* actionNode)
	{
		RotateAction* newAction(xNew0(RotateAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));
		auto rotation = QuaternionRetrieverFactory::Create(
			actionNode->first_node("Rotation"));

		newAction->SetParameters(object, rotation);
		return newAction;
	}
	
	SetOrientationAction::SetOrientationAction() :
		_object(0),
		_rotation(0)
	{ }

	SetOrientationAction::~SetOrientationAction()
	{
		xDeleteSafe(_object);
		xDeleteSafe(_rotation);
	}

	void SetOrientationAction::Execute()
	{
		(static_cast<SceneObject*>(*_object))->SetOrientation(*_rotation);
	}

	void SetOrientationAction::SetParameters(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<Quaternion>* rotationRetriever)
	{
		_object = soRetriever;
		_rotation = rotationRetriever;
	}

	IAction* SetOrientationAction::Factory::Create(XmlNode* actionNode)
	{
		SetOrientationAction* newAction(xNew0(SetOrientationAction));

		auto object = SceneObjectRetrieverFactory::Create(
			actionNode->first_node("Object"));
		auto rotation = QuaternionRetrieverFactory::Create(
			actionNode->first_node("Orientation"));

		newAction->SetParameters(object, rotation);
		return newAction;
	}
}