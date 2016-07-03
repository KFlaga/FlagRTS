#include "SkeletalAnimationDefinitionFactory.h"
#include <LogManager.h>

namespace FlagRTS
{
	SkeletalAnimationDefinitionFactory::SkeletalAnimationDefinitionFactory() :
		IFactoryCast("SkeletalAnimationDefinition")
	{

	}

	SkeletalAnimationDefinitionFactory::~SkeletalAnimationDefinitionFactory()
	{

	}

	IGameObject* SkeletalAnimationDefinitionFactory::Create(XmlNode* animNode)
	{
		SkeletalAnimationDefinition*  sanim = 0;

		try
		{
			sanim = xNew1(SkeletalAnimationDefinition,animNode);
		}
		catch(...) // Failed to create animation - whaterver the reasons, just return invalid pointer
		{
			sanim = 0;
			if(animNode != 0)
				ADD_ERRLOG("Failed to parse skeletal animation node");
			else
				ADD_ERRLOG("Passed nullptr to SkeletalAnimationFactory");
		}

		return sanim;
	}
}
