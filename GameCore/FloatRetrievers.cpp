#include "FloatRetrievers.h"
#include <Exception.h>

namespace FlagRTS
{
	IObjectRetriever<float>* FloatRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			float val = XmlUtility::XmlGetFloat(valueNode, "value", 5);
			return xNew1(FloatFixedRetriever, val);
		}
		else
		{
			CastException((string("Bad float from :") + from).c_str());
			return 0;
		}
	}
}