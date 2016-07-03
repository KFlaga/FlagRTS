#include "AnimationDefinitionFactory.h"
#include "SkeletalAnimationDefinitionFactory.h"
#include "TransformAnimationDefinitionFactory.h"
#include "EmptyAnimationDefinitionFactory.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	AnimationDefinitionFactory::AnimationDefinitionFactory() :
		IFactoryCast("AnimationDefinition")
	{
		_subtypeFactories.insert(std::make_pair(
			"Skeletal", xNew0(SkeletalAnimationDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair(
			"Transform", xNew0(TransformAnimationDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair(
			"Empty", xNew0(EmptyAnimationDefinitionFactory)));
	}

	AnimationDefinitionFactory::~AnimationDefinitionFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->second);
		}
		_subtypeFactories.clear();
	}

	IGameObject* AnimationDefinitionFactory::Create(XmlNode* animNode)
	{
		AnimationDefinition* animDef = 0;

		try
		{
			// First find higher level type of object and corretponding factory
			char* typeName = animNode->first_attribute("type", 4, false)->value();
			auto subFactory = _subtypeFactories.find(typeName);
			if(subFactory != _subtypeFactories.end()) // If subtype factory found
			{
				// Delegate creation of final object to subtype factory
				animDef = (AnimationDefinition*)subFactory->second->Create(animNode);
			}
			else
			{
				CastException((string("Bad SceneObjectDefinition type : ") + typeName).c_str());
			}
		}
		catch(...) // Failed to create animation - whaterver the reasons, just return invalid pointer
		{
			animDef = 0;
			if(animDef != 0)
				ADD_ERRLOG("Failed to parse animation node");
			else
				ADD_ERRLOG("Passed nullptr to AnimationDefinition factory");
		}

		return animDef;
	}
}