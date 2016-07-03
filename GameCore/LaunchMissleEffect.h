#pragma once

#include "IEffect.h"
#include "MissleHolder.h"
#include "ObjectDefinition.h"

namespace FlagRTS
{
	// Interace for LaunchMissleEffect -> as it needs to take track of
	// missle users etc
	class ILaunchMissleEffect : public IEffect
	{
	public:
		ILaunchMissleEffect() { }
		ILaunchMissleEffect(XmlNode* node) :
			IEffect(node)
		{ }

		// Increments Effect's users count
		virtual void AddUser() = 0;
		// Decrements Effect's users count
		virtual void RemoveUser() = 0;
	};

	// Launches arbitrary missle on given target
	// Effect spawns missle and launches it towards target
	// Effect itself is ObjectDefinition as well and can be created via Xml
	// Info on missle launched is stored on MissleHolder
	class LaunchMissleEffect : public ILaunchMissleEffect
	{
	protected:
		MissleHolder _missleStorage;
		Vector3 _spawnOffset; // Offset from caster position ( uses caster orientation )
		Vector3 _targetOffset; // Offset from target to hit ( uses caster orientation )
		bool _applySpawnOffset;
		bool _applyTargetOffset;

	public:
		// Creates Effect using given missle holder, which is copied
		LaunchMissleEffect(const MissleHolder& storage);
		// Creates Effect from Xml
		LaunchMissleEffect(XmlNode* xml);
		~LaunchMissleEffect();

		const MissleHolder& GetMissleHolder() const { return _missleStorage; }

		const Vector3& GetSpawnOffset() const { return _spawnOffset; }
		void SetSpawnOffset(const Vector3& offset) { _spawnOffset = offset; }

		bool GetApplySpawnOffset() const { return _applySpawnOffset; }
		void SetApplySpawnOffset(bool value) { _applySpawnOffset = value; }

		bool GetApplyTargetOffset() const { return _applyTargetOffset; }
		void SetApplyTargetOffset(bool value) { _applyTargetOffset = value; }

		void AddUser();
		void RemoveUser();

		// Gets new Missle from MissleHolder, spawns it on point defined by 'SpawnPosition',
		// and calls Lanuch on it
		// If 'target' == 0, then it is treated as point-target
		// For LaunchMissleEffect 'caster' must be Unit or 0 for trigger-forced effect
		// 'source' is probably Weapon or Command
		// 'target' is PhysicalObject
		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint);
	};

	// Stores one or more LaunchMissleEffects
	// Doesn't own stored events
	class LaunchMissleMultiEffect : public ILaunchMissleEffect
	{
	protected:
		Array<ILaunchMissleEffect*> _effects;

	public:
		LaunchMissleMultiEffect();
		LaunchMissleMultiEffect(XmlNode* xml);
		~LaunchMissleMultiEffect();

		unsigned int GetEffectsCount()
		{
			return _effects.size();
		}

		ILaunchMissleEffect* GetEffectAt(const unsigned int idx)
		{
			return _effects[idx];
		}

		void AddEffect(ILaunchMissleEffect* effect)
		{
			_effects.push_back(effect);
		}

		void RemoveEffect(ILaunchMissleEffect* effect)
		{
			for(unsigned int i = 0; i < _effects.size(); ++i)
			{
				if(_effects[i] == effect)
				{
					_effects.removeAt(i);
					return;
				}
			}
		}

		void AddUser()
		{
			for(unsigned int i = 0; i < _effects.size(); ++i)
			{
				_effects[i]->AddUser();
			}
		}

		void RemoveUser()
		{
			for(unsigned int i = 0; i < _effects.size(); ++i)
			{
				_effects[i]->RemoveUser();
			}
		}

		void ApplyEffect(IGameObject* caster,
			IGameObject* source,
			IGameObject* target,
			const Vector3& targetPoint)
		{
			for(unsigned int i = 0; i < _effects.size(); ++i)
			{
				_effects[i]->ApplyEffect(caster, source, target, targetPoint);
			}
		}
	};
}