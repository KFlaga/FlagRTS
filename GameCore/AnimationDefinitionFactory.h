#pragma once

#include <IFactory.h>
#include "AnimationDefinition.h"
#include <Xml.h>
#include <map>

namespace FlagRTS
{
	class AnimationDefinitionFactory : public IFactoryCast<AnimationDefinition, XmlNode*>
	{
	private:
		typedef std::map<string, IFactory<XmlNode*>*, string_less> FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		AnimationDefinitionFactory();
		~AnimationDefinitionFactory();

		IGameObject* Create(XmlNode* animNode);
	};
}