#pragma once

#include "IEffect.h"

namespace FlagRTS
{
	class SceneObject;
	class SceneObjectDefinition;

	// Effect that creates and spawns SceneObject 
	// Spawn position/orientation determined by EffectTargetType with
	// optional offset/orientation
	// 
	class SpawnObjectEffect : public IEffect
	{
	protected:
		Vector3 _positionOffset;
		Quaternion _orientationOffset;
		SceneObjectDefinition* _objectDef;
		EffectTargetType _spawnPositionType;
		bool _attachToCaster;
		bool _setCastersOwner;

	public:
		SpawnObjectEffect();
		SpawnObjectEffect(XmlNode* xml);

		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint);
	};
}