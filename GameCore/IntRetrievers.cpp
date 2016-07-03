#include "IntRetrievers.h"
#include <Exception.h>
#include "MainEventPool.h"

namespace FlagRTS
{
	IObjectRetriever<int>* IntRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			int val = XmlUtility::XmlGetInt(valueNode, "value");
			return xNew1(IntFixedRetriever, val);
		}
		else if(strcmp(from, "Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(valueNode, "value");
			return xNew1(IntVariableRetriever, varName);
		}
		else
		{
			CastException((string("Bad int from :") + from).c_str());
			return 0;
		}
	}

	IntVariableRetriever::operator int() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<int>();
	}
}