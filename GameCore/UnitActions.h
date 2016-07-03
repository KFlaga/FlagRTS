#pragma once

#include "IAction.h"
#include "DataTypes.h"
#include "IObjectRetriever.h"

namespace FlagRTS
{
	/*class Command;
	class CommandTarget;
	class Weapon;

	class OrderCommandAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<int>* _command;
		IObjectRetriever<CommandTarget*>* _target;
		IObjectRetriever<bool>* _queue;

	public:
		OrderCommandAction(XmlNode* actionNode);
		OrderCommandAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<int>* commandRetriever, 
			IObjectRetriever<CommandTarget*>* targetRetriever,
			IObjectRetriever<bool>* queue);
		~OrderCommandAction();

		void Execute();
	};

	class SetIsSelectedAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<bool>* _value;

	public:
		SetIsSelectedAction(XmlNode* actionNode);
		SetIsSelectedAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<bool>* value);
		~SetIsSelectedAction();

		void Execute();
	};

	class SetIsInteractableAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<bool>* _value;

	public:
		SetIsInteractableAction(XmlNode* actionNode);
		SetIsInteractableAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<bool>* value);
		~SetIsInteractableAction();

		void Execute();
	};
	
	class SetIsImmobilizedAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<bool>* _value;

	public:
		SetIsImmobilizedAction(XmlNode* actionNode);
		SetIsImmobilizedAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<bool>* value);
		~SetIsImmobilizedAction();

		void Execute();
	};

	class SetIsDisarmedAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<bool>* _value;

	public:
		SetIsDisarmedAction(XmlNode* actionNode);
		SetIsDisarmedAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<bool>* value);
		~SetIsDisarmedAction();

		void Execute();
	};

	class SetIsSiliencedAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<bool>* _value;

	public:
		SetIsSiliencedAction(XmlNode* actionNode);
		SetIsSiliencedAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<bool>* value);
		~SetIsSiliencedAction();

		void Execute();
	};

	class SetConstructionProgressAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<float>* _value;

	public:
		SetConstructionProgressAction(XmlNode* actionNode);
		SetConstructionProgressAction(
			IObjectRetriever<SceneObject*>* soRetriever, 
			IObjectRetriever<float>* value);
		~SetConstructionProgressAction();

		void Execute();
	};
	
	class KillAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<int>* _reason;
		IObjectRetriever<bool>* _instant;

	public:
		KillAction(XmlNode* actionNode);
		KillAction(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<int>* reason,
			IObjectRetriever<bool>* instant);
		~KillAction();

		void Execute();
	};
	
	class InflictDamageAction : public SceneObjectAction
	{
	private:
		IObjectRetriever<Weapon*>* _weapon;
		IObjectRetriever<int>* _damage;

	public:
		InflictDamageAction(XmlNode* actionNode);
		InflictDamageAction(
			IObjectRetriever<SceneObject*>* soRetriever,
			IObjectRetriever<int>* damage,
			IObjectRetriever<Weapon*>* weapon);
		~InflictDamageAction();

		void Execute();
	};*/
}