#pragma once

#include "ObjectDefinition.h"
#include "MissleDefinition.h"
#include <Event.h>
#include <Xml.h>

namespace Ogre
{
	class Image;
}

namespace FlagRTS
{
	class Weapon;
	class CommandTarget;
	class Unit;
	class ILaunchMissleEffect;

	namespace WeaponTypes
	{
		enum WeaponType
		{
			Melee, // Melee attacks fires invisible missles , which does damange on collision
			Missle, // Attack causes to fire missle, which does actual damage
			Target, // Instant attacks
			Custom // No base type, all weapon events from script
		};

		WeaponType Parse(const string& type);
	}
	typedef WeaponTypes::WeaponType WeaponType;

	class WeaponStats
	{
	public:
		int Damage;
		int Range;
		int AttacksCount;
		float AttackSpeed;
		float AttacksDelay;

		void Parse(XmlNode* node);
	};

	class WeaponDefinition : public ObjectDefinition
	{
	protected:
		string _iconName;
		string _guiName;

		WeaponType _type;
		WeaponStats _stats;

		IEffect* _onAttackEffect; // Effect activated on attack ( for uses other missles )
		ILaunchMissleEffect* _missleEffect; // Effect for launching missle for missle weapons

		Degree _attackAngle; // Angle of cone within which unit can attack w/o rotating

		string _actionName; // Action name associated with this weapon ( unit can have different animations for different weapons )
		float _attackTime; // Part of attack animation in which actual OnAttack event should be fired

		int _validTargets;

	public:
		WeaponDefinition();
		WeaponDefinition(XmlNode* defNode);

		const string& GetActionName() const { return _actionName; }
		void SetActionName(const string& name) { _actionName = name; }

		float GetAttackTime() const { return _attackTime; }
		void SetAttackTIme(float time) { _attackTime = time; }

		Degree GetAttackAngle() const { return _attackAngle; }
		void SetAttackAngle(Degree angle) { _attackAngle = angle; }

		void SetStats(const WeaponStats& stats) { _stats = stats; }
		const WeaponStats& GetStats() const { return _stats; }
		WeaponStats& GetStats() { return _stats; }

		void SetIcon(const string& icon) { _iconName = icon; }
		const string& GetIcon() const { return _iconName; }

		void SetGuiName(const string& name) { _guiName = name; }
		const string& GetGuiName() const { return _guiName; }

		void SetWeaponType(WeaponType value) { _type = value; }
		WeaponType GetWeaponType() const { return _type; }

		IEffect* GetOnAttackEffect() { return _onAttackEffect; }
		void SetOnAttackEffect(IEffect* effect) { _onAttackEffect = effect; }
		
		ILaunchMissleEffect* GetLaunchMissleEffect() { return _missleEffect; }
		void SetLaunchMissleEffect(ILaunchMissleEffect* effect) { _missleEffect = effect; }

		int GetAvailableTargets() const { return _validTargets; }
		void SetAvailableTargets(int targets) { _validTargets = targets; }
	};
}