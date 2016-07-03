#pragma once

#include <IFactory.h>
#include "TransformAnimationDefinition.h"
#include <Xml.h>

namespace FlagRTS
{
	class TransformAnimationDefinitionFactory : public IFactoryCast<TransformAnimationDefinition, XmlNode*>
	{
	public:
		TransformAnimationDefinitionFactory();
		~TransformAnimationDefinitionFactory();

		IGameObject* Create(XmlNode* animNode);
	};
}