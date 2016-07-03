#pragma once 

#include "IEffect.h"
#include <Array.h>
#include "CollisionArea.h"

namespace FlagRTS
{
	// Fires multiple effects
	class MultiEffect : public IEffect
	{
	protected:
		Array<IEffect*> _effects;

	public:
		MultiEffect();
		MultiEffect(XmlNode* xml);

		~MultiEffect();
		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint);

		void AddEffect(IEffect* effect);
		void RemoveEffect(IEffect* effect);
	};

	// Fires effect for all units in area
	class IAreaEffect : public IEffect
	{
	protected:
		IEffect* _effect;
		CollisionArea _area;

	public:
		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint);
	};
}