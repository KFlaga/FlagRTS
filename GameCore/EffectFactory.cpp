#include "EffectFactory.h"
#include "LaunchMissleEffect.h"
#include "InflictDamagaEffect.h"
#include "BaseEffects.h"
#include "SpawnObjectEffect.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(LaunchMissleEffectFactory, 
		LaunchMissleEffect, 
		"LaunchMissleEffect", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(LaunchMissleMultiEffectFactory, 
		LaunchMissleMultiEffect, 
		"LaunchMissleMultiEffect", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(InflictDamageEffectFactory, 
		InflictDamageEffect, 
		"InflictDamageEffect", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(MultiEffectFactory, 
		MultiEffect, 
		"MultiEffect", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(SpawnObjectEffectFactory, 
		SpawnObjectEffect, 
		"SpawnObjectEffect", 
		XmlNode*);

	EffectFactory::EffectFactory() :
		IFactoryCast("IEffect")
	{
		_subtypeFactories.insert(std::make_pair("LaunchMissle",
			xNew0(LaunchMissleEffectFactory)));

		_subtypeFactories.insert(std::make_pair("LaunchMissleMulti",
			xNew0(LaunchMissleMultiEffectFactory)));
		
		_subtypeFactories.insert(std::make_pair("InflictDamage",
			xNew0(InflictDamageEffectFactory)));
		
		_subtypeFactories.insert(std::make_pair("Multi",
			xNew0(MultiEffectFactory)));
		
		_subtypeFactories.insert(std::make_pair("SpawnObject",
			xNew0(SpawnObjectEffectFactory)));
	}

	EffectFactory::~EffectFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* EffectFactory::Create(XmlNode* objNode)
	{
		IGameObject* obj = 0;

		// First find higher level type of object and corretponding factory
		char* typeName = objNode->first_attribute("type", 4, false)->value();
		auto subFactory = _subtypeFactories.find(typeName);
		if(subFactory != _subtypeFactories.end()) // If subtype factory found
		{
			// Delegate creation of final object to subtype factory
			obj = subFactory->Value->Create(objNode);
		}
		else
		{
			CastException((string("Bad Effect type : ") + typeName).c_str());
		}

		return obj;
	}
}
