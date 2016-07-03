#include "VariableRetrievers.h"
#include <Exception.h>
#include "MainEventPool.h"

namespace FlagRTS
{
	IObjectRetriever<Variable>* VariableRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Name") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(valueNode, "value");
			return xNew1(VariableFromNameRetriever, varName);
		}
		else
		{
			CastException((string("Bad int from :") + from).c_str());
			return 0;
		}
	}

	VariableFromNameRetriever::operator Variable() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_name);
		return var;
	}
}