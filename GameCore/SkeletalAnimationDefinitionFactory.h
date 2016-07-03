#pragma once

#include <IFactory.h>
#include "SkeletalAnimationDefinition.h"
#include <Xml.h>

namespace FlagRTS
{
	class SkeletalAnimationDefinitionFactory : public IFactoryCast<SkeletalAnimationDefinition, XmlNode*>
	{
	public:
		SkeletalAnimationDefinitionFactory();
		~SkeletalAnimationDefinitionFactory();

		IGameObject* Create(XmlNode* animNode);
	};
}