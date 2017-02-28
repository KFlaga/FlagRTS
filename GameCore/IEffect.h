#pragma once

#include "ObjectDefinition.h"
#include "IGameObject.h"

namespace FlagRTS
{
	class Unit;
	class SceneObject;

	namespace EffectTargets
	{
		enum EffectTargetType : int
		{
			None = 0,
			Caster, // Target is effect caster ( etc. lifesteal )
			CasterCenter, // Target is caster center ( etc. self-aoe spells )
			Source, // Target is effect source ( like missle ) ( no idea now )
			SourceCenter, // Target is effect source center ( like missle ) ( etc. spawn explosion )
			Target, // Target is effect target object
			TargetCenter, // Target is effect target center
			TargetPoint, // Target is point in passed in targetPoint
			Absolute, // Absolute coords are used ( only offset is used if applicable )
		};

		EffectTargetType ParseEffectTargetType(const char* name);
	};
	typedef EffectTargets::EffectTargetType EffectTargetType;

	// Base class for all Effects
	// Effect is arbitrary action taken when somthing happend and it should have some effect on units etc.
	// They inherit from ObjectDefinitions and are created by DefinitionCreator as well as
	// obtained from here -> only one copy of effect exists and should be used by all objects that needs
	// to apply this effect
	class IEffect : public ObjectDefinition
	{
	public:
		IEffect() { }
		IEffect(XmlNode* node) :
			ObjectDefinition(node)
		{ }

		virtual ~IEffect() { }

		virtual void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint) = 0;
	};
}