#include "ISceneObjectRetriever.h"
#include "UnitRetrievers.h"
#include <Exception.h>

namespace FlagRTS
{
	ISceneObjectRetriever* SceneObjectRetrieverFactory::Create(XmlNode* soNode)
	{
		// Find object type
		string type = XmlUtility::XmlGetString(soNode, "type", 4);

		if(type.compare("Unit") == 0)
		{
			return reinterpret_cast<ISceneObjectRetriever*>(UnitRetrieverFactory::Create(soNode));
		}
		else
		{
			CastException((string("Bad SceneObject type :") + type).c_str());
			return 0;
		}
	}
}