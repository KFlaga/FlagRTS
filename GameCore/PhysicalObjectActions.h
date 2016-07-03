//#pragma once
//
//#include "SceneObjectAction.h"
//
//namespace FlagRTS
//{
//	class PhysicalObject;
//
//	class ChangeAnimationAction : public SceneObjectAction
//	{
//	private:
//		IObjectRetriever<size_t>* _animState;
//		IObjectRetriever<const string&>* _actionName;
//
//	public:
//		ChangeAnimationAction(XmlNode* actionNode);
//		ChangeAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever);
//		~ChangeAnimationAction();
//
//		void Execute();
//	};
//
//	class AddAnimationAction : public SceneObjectAction
//	{
//	private:
//		IObjectRetriever<size_t>* _animState;
//		IObjectRetriever<const string&>* _actionName;
//
//	public:
//		AddAnimationAction(XmlNode* actionNode);
//		AddAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever);
//		~AddAnimationAction();
//
//		void Execute();
//	};
//
//	class EndAnimationAction : public SceneObjectAction
//	{
//	private:
//		IObjectRetriever<size_t>* _animState;
//		IObjectRetriever<const string&>* _actionName;
//
//	public:
//		EndAnimationAction(XmlNode* actionNode);
//		EndAnimationAction(
//			IObjectRetriever<SceneObject*>*  soRetriever, 
//			IObjectRetriever<size_t>* stateRetriever, 
//			IObjectRetriever<const string&>* actionRetriever);
//		~EndAnimationAction();
//
//		void Execute();
//	};
//
//	class SetIsSelectableAction : public SceneObjectAction
//	{
//	private:
//		IObjectRetriever<bool>* _value;
//
//	public:
//		SetIsSelectableAction(XmlNode* actionNode);
//		SetIsSelectableAction(
//			IObjectRetriever<SceneObject*>* soRetriever, 
//			IObjectRetriever<bool>* valueRetriever);
//		~SetIsSelectableAction();
//
//		void Execute();
//	};
//
//	class SetIsHoverableAction : public SceneObjectAction
//	{
//	private:
//		IObjectRetriever<bool>* _value;
//
//	public:
//		SetIsHoverableAction(XmlNode* actionNode);
//		SetIsHoverableAction(
//			IObjectRetriever<SceneObject*>* soRetriever, 
//			IObjectRetriever<bool>* valueRetriever);
//		~SetIsHoverableAction();
//
//		void Execute();
//	};
//}