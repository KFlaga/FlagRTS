#include "EmptyAnimationDefinitionFactory.h"
#include <LogManager.h>

namespace FlagRTS
{
	EmptyAnimationDefinitionFactory::EmptyAnimationDefinitionFactory() :
		IFactoryCast("EmptyAnimationDefinition")
	{

	}

	EmptyAnimationDefinitionFactory::~EmptyAnimationDefinitionFactory()
	{

	}

	IGameObject* EmptyAnimationDefinitionFactory::Create(XmlNode* animNode)
	{
		EmptyAnimationDefinition*  sanim = 0;

		/*try
		{*/
			sanim = xNew1(EmptyAnimationDefinition,animNode);
		//}
		//catch(...) // Failed to create animation - whaterver the reasons, just return invalid pointer
		//{
		//	sanim = 0;
		//	if(animNode != 0)
		//		ADD_ERRLOG("Failed to parse empty animation node");
		//	else
		//		ADD_ERRLOG("Passed nullptr to EmptyAnimationDefinitionFactory");
		//}

		return sanim;
	}
}
