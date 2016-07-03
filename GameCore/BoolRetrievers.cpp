#include "BoolRetrievers.h"
#include <Exception.h>
#include "MainEventPool.h"

namespace FlagRTS
{
	IObjectRetriever<bool>* BoolRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			bool val = XmlUtility::XmlGetBool(valueNode, "value", 5);
			return xNew1(BoolFixedRetriever, val);
		}
		else if(strcmp(from, "And") == 0)
		{
			// Should be 2 nodes Value with from etc
			XmlNode* val1Node = valueNode->first_node("Value");
			XmlNode* val2Node = val1Node->next_sibling("Value");
			IObjectRetriever<bool>* val1 = Create(val1Node);
			IObjectRetriever<bool>* val2 = Create(val2Node);

			return xNew2(BoolAndRetriever, val1, val2);
		}
		else if(strcmp(from, "Or") == 0)
		{
			// Should be 2 nodes Value with from etc
			XmlNode* val1Node = valueNode->first_node("Value");
			XmlNode* val2Node = val1Node->next_sibling("Value");
			IObjectRetriever<bool>* val1 = Create(val1Node);
			IObjectRetriever<bool>* val2 = Create(val2Node);

			return xNew2(BoolOrRetriever, val1, val2);
		}
		else if(strcmp(from, "Not") == 0)
		{
			// Should be 2 nodes Value with from etc
			XmlNode* val1Node = valueNode->first_node("Value");
			IObjectRetriever<bool>* val1 = Create(val1Node);

			return xNew1(BoolNotRetriever, val1);
		}
		else if(strcmp(from, "Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(valueNode, "value");
			return xNew1(BoolVariableRetriever, varName);
		}
		else
		{
			CastException((string("Bad from :") + from).c_str());
			return 0;
		}
	}

	BoolAndRetriever::~BoolAndRetriever()
	{
		xDelete(_val1);
		xDelete(_val2);
	}

	BoolOrRetriever::~BoolOrRetriever()
	{
		xDelete(_val1);
		xDelete(_val2);
	}

	BoolNotRetriever::~BoolNotRetriever()
	{
		xDelete(_val1);
	}

	BoolVariableRetriever::operator bool() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<bool>();
	}
}