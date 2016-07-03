#include "BaseEffects.h"
#include "GameWorld.h"
#include "MainGameObjectPool.h"

namespace FlagRTS
{
	MultiEffect::MultiEffect()
	{

	}

	MultiEffect::MultiEffect(XmlNode* node) :
		IEffect(node)
	{
		// Effects should be in stored inside Effects node
		XmlNode* allEffectsNode = node->first_node("Effects");
		XmlNode* effectNode = allEffectsNode->first_node("Effect");
		while(effectNode != 0)
		{
			string effectName = XmlUtility::XmlGetString(effectNode, "name");

			auto getEffect = [this, effectName]() 
			{
				IEffect* effect = static_cast<IEffect*>(
					GameWorld::GlobalWorld->GetGameObjectDefinition("Effect", effectName));
				_effects.push_back(effect);
			};
			MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
				xNew1(DelegateEventHandler<decltype(getEffect)>, getEffect);

			effectNode = effectNode->next_sibling();
		}
	}

	MultiEffect::~MultiEffect()
	{
	}

	void MultiEffect::ApplyEffect(IGameObject* caster,
		IGameObject* source,
		IGameObject* target,
		const Vector3& targetPoint)
	{
		for(unsigned int i = 0; i < _effects.size(); ++i)
		{
			_effects[i]->ApplyEffect(caster, source, target, targetPoint);
		}
	}

	void MultiEffect::AddEffect(IEffect* effect)
	{
		_effects.push_back(effect);
	}

	void MultiEffect::RemoveEffect(IEffect* effect)
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

	namespace EffectTargets
	{
		std::map<string, EffectTargetType>& GetEffectTargetNamesMap()
		{
			static std::map<string, EffectTargetType> map;
			map["None"] = None;
			map["Caster"] = Caster;
			map["CasterCenter"] = CasterCenter;
			map["Source"] = Source;
			map["SourceCenter"] = SourceCenter;
			map["Target"] = Target;
			map["TargetCenter"] = TargetCenter;
			map["TargetPoint"] = TargetPoint;
			map["Absolute"] = Absolute;
			return map;
		}

		EffectTargetType ParseEffectTargetType(const char* name)
		{
			static std::map<string, EffectTargetType>& map = GetEffectTargetNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return None;
		}
	};
}
