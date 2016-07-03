//#include "UnitActions.h"
//#include "ISceneObjectRetriever.h"
//#include "BoolRetrievers.h"
//#include "IntRetrievers.h"
//#include "FloatRetrievers.h"
//#include "Unit.h"
//#include "SceneObjectStateRetrievers.h"
//#include "StringRetrievers.h"
//#include "CommandQueue.h"
//#include "CommandTypeRetrievers.h"
//#include "CommandTargetRetrievers.h"
//#include "KillReasonRetriever.h"
//#include "WeaponRetrievers.h"
//#include "DamageRetrievers.h"
//
//#include "GameWorld.h"
//
//namespace FlagRTS
//{
//	OrderCommandAction::OrderCommandAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_command = CommandTypeRetrieverFactory::Create(
//			actionNode->first_node("Command"));
//		_target = CommandTargetRetrieverFactory::Create(
//			actionNode->first_node("Target"));
//		_queue = BoolRetrieverFactory::Create(
//			actionNode->first_node("Queue"));
//	}
//
//	OrderCommandAction::OrderCommandAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<int>* commandRetriever, 
//		IObjectRetriever<CommandTarget*>* targetRetriever,
//		IObjectRetriever<bool>* queue) :
//	SceneObjectAction(soRetriever),
//		_command(commandRetriever),
//		_target(targetRetriever),
//		_queue(queue)
//	{ }
//
//	OrderCommandAction::~OrderCommandAction()
//	{
//	}
//
//	void OrderCommandAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->GetCommandQueue()
//			->ExecuteCommand(*_command, *_target, *_queue);
//	}
//
//
//	SetIsSelectedAction::SetIsSelectedAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = BoolRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetIsSelectedAction::SetIsSelectedAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<bool>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetIsSelectedAction::~SetIsSelectedAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetIsSelectedAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->SetIsSelected(*_value);
//	}
//
//	SetIsInteractableAction::SetIsInteractableAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = BoolRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetIsInteractableAction::SetIsInteractableAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<bool>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetIsInteractableAction::~SetIsInteractableAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetIsInteractableAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->SetIsInteractable(*_value);
//	}
//	
//	SetIsImmobilizedAction::SetIsImmobilizedAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = BoolRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetIsImmobilizedAction::SetIsImmobilizedAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<bool>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetIsImmobilizedAction::~SetIsImmobilizedAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetIsImmobilizedAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->SetIsImmobilized(*_value);
//	}
//	
//	SetIsDisarmedAction::SetIsDisarmedAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = BoolRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetIsDisarmedAction::SetIsDisarmedAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<bool>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetIsDisarmedAction::~SetIsDisarmedAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetIsDisarmedAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->SetIsDisarmed(*_value);
//	}
//	
//	SetIsSiliencedAction::SetIsSiliencedAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = BoolRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetIsSiliencedAction::SetIsSiliencedAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<bool>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetIsSiliencedAction::~SetIsSiliencedAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetIsSiliencedAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->SetIsSilienced(*_value);
//	}
//	
//	SetConstructionProgressAction::SetConstructionProgressAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_value = FloatRetrieverFactory::Create(
//			actionNode->first_node("Value"));
//	}
//
//	SetConstructionProgressAction::SetConstructionProgressAction(
//		IObjectRetriever<SceneObject*>* soRetriever, 
//		IObjectRetriever<float>* value) :
//	SceneObjectAction(soRetriever),
//		_value(value)
//	{ }
//
//	SetConstructionProgressAction::~SetConstructionProgressAction()
//	{
//		xDelete(_value);
//	}
//
//	void SetConstructionProgressAction::Execute()
//	{
//		Unit* unit = (static_cast<Unit*>(_object->operator SceneObject*()));
//		unit->SetConstructionProgress(*_value);
//		if( *_value >= 1.f )
//		{
//			unit->ConstructionFinished().Fire((Unit*)0, unit, true);
//		}
//	}
//	
//	KillAction::KillAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_reason = KillReasonRetrieverFactory::Create(
//			actionNode->first_node("Reason"));
//		_instant = BoolRetrieverFactory::Create(
//			actionNode->first_node("Instant"));
//	}
//
//	KillAction::KillAction(
//			IObjectRetriever<SceneObject*>* soRetriever,
//			IObjectRetriever<int>* reason,
//			IObjectRetriever<bool>* instant) :
//	SceneObjectAction(soRetriever),
//		_reason(reason),
//		_instant(instant)
//	{ }
//
//	KillAction::~KillAction()
//	{
//		xDelete(_reason);
//		xDelete(_instant);
//	}
//
//	void KillAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->KillUnit(*_reason, *_instant);
//	}
//		
//	InflictDamageAction::InflictDamageAction(XmlNode* actionNode) :
//		SceneObjectAction(actionNode)
//	{
//		_weapon = WeaponRetrieverFactory::Create(
//			actionNode->first_node("Reason"));
//		_damage = DamageRetrieverFactory::Create(
//			actionNode->first_node("Instant"));
//	}
//
//	InflictDamageAction::InflictDamageAction(
//			IObjectRetriever<SceneObject*>* soRetriever,
//			IObjectRetriever<int>* damage,
//			IObjectRetriever<Weapon*>* weapon) :
//	SceneObjectAction(soRetriever),
//		_weapon(weapon),
//		_damage(damage)
//	{ }
//
//	InflictDamageAction::~InflictDamageAction()
//	{
//		xDelete(_weapon);
//		xDelete(_damage);
//	}
//
//	void InflictDamageAction::Execute()
//	{
//		(static_cast<Unit*>(_object->operator SceneObject*()))->InflictDamage(*_damage, *_weapon);
//	}
//}