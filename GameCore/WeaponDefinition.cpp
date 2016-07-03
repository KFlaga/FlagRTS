#include "WeaponDefinition.h"
#include "Weapon.h"
#include <OgreImage.h>
#include <LocalizationManager.h>
#include "UnitClass.h"
#include "GameWorld.h"
#include "MainGameObjectPool.h"
#include "LaunchMissleEffect.h"

namespace FlagRTS
{
	WeaponDefinition::WeaponDefinition() :
		ObjectDefinition(),
		_onAttackEffect(0),
		_missleEffect(0)
	{
		SetFinalType(GetTypeId<Weapon>());
		SetFinalTypeName("Weapon");
	}

	WeaponDefinition::WeaponDefinition(XmlNode* node) :
		ObjectDefinition(node),
		_onAttackEffect(0),
		_missleEffect(0)
	{
		SetFinalType(GetTypeId<Weapon>());
		SetFinalTypeName("Weapon");

		_iconName = XmlUtility::XmlGetString(node->first_node("Icon",4),"name",4);

		_stats.Parse(node->first_node("Stats",5));
		_type = WeaponTypes::Parse(XmlUtility::XmlGetString(node->first_node("Type",4),"value",5));

		_actionName = XmlUtility::XmlGetString(node->first_node("Action", 6),"value",5);
		_attackAngle = Degree((float)XmlUtility::XmlGetInt(node->first_node("AttackAngle",11),"value",5));
		_attackTime = XmlUtility::XmlGetFloat(node->first_node("AttackTime",10),"value",5);

		// _validTargets = UnitClasses::ParseAttackTargets(XmlUtility::XmlGetString(node->first_node("Targets",7),"value",5));

		XmlNode* attackEffectNode = node->first_node("OnAttack");
		if(attackEffectNode != 0)
		{
			string effectName = XmlUtility::XmlGetString(attackEffectNode, "effect");

			auto getEffect = [this, effectName]() 
			{
				IEffect* effect = static_cast<IEffect*>(
					GameWorld::GlobalWorld->GetGameObjectDefinition("Effect", effectName));
				_onAttackEffect = effect;
			};
			MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
				xNew1(DelegateEventHandler<decltype(getEffect)>, getEffect);
		}

		if( _type == WeaponTypes::Missle )
		{
			XmlNode* missleEffectNode = node->first_node("Missle");
			if(missleEffectNode != 0)
			{
				string effectName = XmlUtility::XmlGetString(missleEffectNode, "effect");

				auto getEffect = [this, effectName]() 
				{
					ILaunchMissleEffect* effect = static_cast<ILaunchMissleEffect*>(
						GameWorld::GlobalWorld->GetGameObjectDefinition("Effect", effectName));
					_missleEffect = effect;
				};
				MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
					xNew1(DelegateEventHandler<decltype(getEffect)>, getEffect);
			}
		}
	}


	WeaponType WeaponTypes::Parse(const string& type)
	{
		if(type.compare("Melee") == 0)
			return WeaponTypes::Melee;
		if(type.compare("Missle") == 0)
			return WeaponTypes::Missle;
		if(type.compare("Target") == 0)
			return WeaponTypes::Target;

		return WeaponTypes::Custom;
	}

	void WeaponStats::Parse(XmlNode* node)
	{
		Range = XmlUtility::XmlGetInt(node, "range", 5);
		Damage = XmlUtility::XmlGetInt(node, "damage", 6);
		AttacksCount = XmlUtility::XmlGetInt(node, "count", 5);
		AttacksDelay = XmlUtility::XmlGetFloat(node, "delay", 5) * 1000.f;

		AttackSpeed = 1/AttacksDelay;
	}
}
