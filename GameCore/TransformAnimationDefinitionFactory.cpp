#include "TransformAnimationDefinitionFactory.h"
#include <LogManager.h>

namespace FlagRTS
{
	TransformAnimationDefinitionFactory::TransformAnimationDefinitionFactory() :
		IFactoryCast("TransformAnimationDefinition")
	{

	}

	TransformAnimationDefinitionFactory::~TransformAnimationDefinitionFactory()
	{

	}

	IGameObject* TransformAnimationDefinitionFactory::Create(XmlNode* animNode)
	{
		TransformAnimationDefinition*  sanim = 0;

		/*try
		{*/
			sanim = xNew1(TransformAnimationDefinition,animNode);
		//}
		//catch(...) // Failed to create animation - whaterver the reasons, just return invalid pointer
		//{
		//	sanim = 0;
		//	if(animNode != 0)
		//		ADD_ERRLOG("Failed to parse transform animation node");
		//	else
		//		ADD_ERRLOG("Passed nullptr to TransformAnimationDefinitionFactory");
		//}

		return sanim;
	}
}
