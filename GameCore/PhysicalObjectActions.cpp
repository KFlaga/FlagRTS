//#include "PhysicalObjectActions.h"
//#include "ISceneObjectRetriever.h"
//#include "BoolRetrievers.h"
//#include "IntRetrievers.h"
//#include "PhysicalObject.h"
//#include "AnimationController.h"
//#include "SceneObjectStateRetrievers.h"
//#include "StringRetrievers.h"
//
//#include "GameWorld.h"
//
//namespace FlagRTS
//{
//	ChangeAnimationAction::ChangeAnimationAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		// Move action should have "Vector" node with point defintion
//		// in attribute "from" stores method to create vector3
//		_animState = SceneObjectStateRetrieverFactory::Create(
//			actionNode->first_node("State"));
//
//		_actionName = StringRetrieverFactory::Create(
//			actionNode->first_node("State"));
//	}
//
//	ChangeAnimationAction::ChangeAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever) :
//	SceneObjectAction(soRetriever),
//		_animState(stateRetriever),
//		_actionName(actionRetriever)
//	{ }
//
//	ChangeAnimationAction::~ChangeAnimationAction()
//	{
//		xDelete(_animState);
//		xDelete(_actionName);
//	}
//
//	void ChangeAnimationAction::Execute()
//	{
//		(static_cast<PhysicalObject*>(_object->operator SceneObject*()))->GetAnimController().
//			ChangeAnimation(*_animState, string(*_actionName));
//	}
//
//	AddAnimationAction::AddAnimationAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		// Move action should have "Vector" node with point defintion
//		// in attribute "from" stores method to create vector3
//		_animState = SceneObjectStateRetrieverFactory::Create(
//			actionNode->first_node("State"));
//
//		_actionName = StringRetrieverFactory::Create(
//			actionNode->first_node("State"));
//	}
//
//	AddAnimationAction::AddAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever) :
//	SceneObjectAction(soRetriever),
//		_animState(stateRetriever),
//		_actionName(actionRetriever)
//	{ }
//
//	AddAnimationAction::~AddAnimationAction()
//	{
//		xDelete(_animState);
//		xDelete(_actionName);
//	}
//
//	void AddAnimationAction::Execute()
//	{
//		(static_cast<PhysicalObject*>(_object->operator SceneObject*()))->GetAnimController().
//			AddAnimation(*_animState, string(*_actionName));
//	}
//
//	EndAnimationAction::EndAnimationAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		// Move action should have "Vector" node with point defintion
//		// in attribute "from" stores method to create vector3
//		_animState = SceneObjectStateRetrieverFactory::Create(
//			actionNode->first_node("State"));
//
//		_actionName = StringRetrieverFactory::Create(
//			actionNode->first_node("State"));
//	}
//
//	EndAnimationAction::EndAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever) :
//	SceneObjectAction(soRetriever),
//		_animState(stateRetriever),
//		_actionName(actionRetriever)
//	{ }
//
//	EndAnimationAction::~EndAnimationAction()
//	{
//		xDelete(_animState);
//		xDelete(_actionName);
//	}
//
//	void EndAnimationAction::Execute()
//	{
//		(static_cast<PhysicalObject*>(_object->operator SceneObject*()))->GetAnimController().
//			EndAnimation(*_animState, string(*_actionName));
//	}
//}