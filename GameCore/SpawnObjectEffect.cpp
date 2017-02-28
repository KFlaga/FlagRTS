#include "SpawnObjectEffect.h"
#include "SceneObject.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "IGameObjectPool.h"
#include "ISceneObjectSpawner.h"

namespace FlagRTS
{
	SpawnObjectEffect::SpawnObjectEffect()
	{
		SetFinalType(GetTypeId<SpawnObjectEffect>());
		SetFinalTypeName("Effect");
	}

	SpawnObjectEffect::SpawnObjectEffect(XmlNode* node) :
		IEffect(node)
	{
		SetFinalType(GetTypeId<SpawnObjectEffect>());
		SetFinalTypeName("Effect");

		_positionOffset = XmlUtility::XmlGetXYZIfExists(node->first_node("Position"), Vector3::ZERO);
		_orientationOffset = XmlUtility::XmlGetQuaternionIfExists(node->first_node("Orientation"), Quaternion::IDENTITY);
		_spawnPositionType = EffectTargets::ParseEffectTargetType(XmlUtility::XmlGetStringIfExists(
			node->first_node("SpawnPositionType"), "value", "Caster"));
		_attachToCaster = XmlUtility::XmlGetBoolIfExists(node->first_node("AttachToCaster"), "value", false);
		_setCastersOwner =  XmlUtility::XmlGetBoolIfExists(node->first_node("SetCastersOwner"), "value", false);

		XmlNode* objectNode = node->first_node("Object");
		string defName = XmlUtility::XmlGetString(objectNode, "definition");
		string typeName = XmlUtility::XmlGetString(objectNode, "type");

		auto getDefinition = [this, defName, typeName](IObjectDefinitionManager* mgr) 
		{
			_objectDef = static_cast<SceneObjectDefinition*>(
				mgr->GetObjectDefinitionByName(typeName, defName));
		};
		typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
		GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
			xNew1(DefinitionsLoadedHandler, getDefinition);

	}

	void SpawnObjectEffect::ApplyEffect(IGameObject* caster,
		IGameObject* source,
		IGameObject* target,
		const Vector3& targetPoint)
	{
		// Create object
		SceneObject* object = static_cast<SceneObject*>(GameInterfaces::GetGameObjectPool()->
			Create(_objectDef, 
			_setCastersOwner ? caster->GetOwner() : NEUTRAL_PLAYERNUM));

		if(_attachToCaster && caster != 0)
		{
			// Assume _spawnPositionType = Caster
			GameInterfaces::GetSceneObjectSpawner()->SpawnSceneObject(object, 
				SpawnInfo(_orientationOffset, _positionOffset, false));

			if(object->GetSceneNode()->getParent() != 0)
				object->GetSceneNode()->getParent()->removeChild(object->GetSceneNode());
			static_cast<SceneObject*>(caster)->AttachChild(object, true);
			return;
		}

		// Calculate spawn info
		Vector3 spawnPosition(0.f,0.f,0.f);
		Quaternion spawnOrient(1.f,0.f,0.f,0.f);
		switch (_spawnPositionType)
		{
		case EffectTargets::Caster:
		case EffectTargets::CasterCenter:
			if( caster != 0 )
			{
				spawnPosition = static_cast<SceneObject*>(caster)->GetPositionAbsolute();
				spawnOrient = static_cast<SceneObject*>(caster)->GetOrientationAbsolute();
			}
			break;
		case EffectTargets::Target:
			if( target != 0 )
			{
				spawnPosition = static_cast<SceneObject*>(target)->GetPositionAbsolute() + targetPoint;
				spawnOrient = static_cast<SceneObject*>(target)->GetOrientationAbsolute();
			}
			break;
		case EffectTargets::TargetCenter:
			if( target != 0 )
			{
				spawnPosition = static_cast<SceneObject*>(target)->GetPositionAbsolute();
				spawnOrient = static_cast<SceneObject*>(target)->GetOrientationAbsolute();
			}
			break;
		case EffectTargets::TargetPoint:
			spawnPosition = targetPoint;
			break;
		case EffectTargets::Source:
		case EffectTargets::SourceCenter:
			if( source != 0 )
			{
				spawnPosition = static_cast<SceneObject*>(source)->GetPositionAbsolute();
				spawnOrient = static_cast<SceneObject*>(source)->GetOrientationAbsolute();
			}
			break;
		case EffectTargets::Absolute:
		default:
			break;
		}

		spawnOrient = spawnOrient * _orientationOffset;
		spawnPosition += spawnOrient * _positionOffset;

		// Spawn object
		GameInterfaces::GetSceneObjectSpawner()->SpawnSceneObject(object, 
			SpawnInfo(spawnOrient, spawnPosition, false));
	}
}