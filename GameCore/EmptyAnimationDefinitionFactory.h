#pragma once

#include <IFactory.h>
#include "EmptyAnimationDefinition.h"
#include <Xml.h>

namespace FlagRTS
{
	class EmptyAnimationDefinitionFactory : public IFactoryCast<EmptyAnimationDefinition, XmlNode*>
	{
	public:
		EmptyAnimationDefinitionFactory();
		~EmptyAnimationDefinitionFactory();

		IGameObject* Create(XmlNode* animNode);
	};
}