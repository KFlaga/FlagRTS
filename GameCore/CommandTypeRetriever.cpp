#include "CommandTypeRetrievers.h"
#include "IntRetrievers.h"
#include <Exception.h>
#include "Command.h"
#include "UnitRetrievers.h"

namespace FlagRTS
{
	IObjectRetriever<int>* CommandTypeRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			int val = XmlUtility::XmlGetInt(valueNode, "value", 5);
			return xNew1(IntFixedRetriever, val);
		}
		else if(strcmp(from, "OrderName") == 0)
		{
			const char* name = XmlUtility::XmlGetString(valueNode, "value", 5);
			return xNew1(CommandTypeFixedRetriever, OrderTypes::ParseOrderType(name));
		}
		else if(strcmp(from, "UnitCommandName") == 0)
		{
			const char* name = XmlUtility::XmlGetString(valueNode, "value", 5);
			// Should have node Unit
			IObjectRetriever<Unit*>* unit = UnitRetrieverFactory::Create(
				valueNode->first_node("Unit", 4));

			return xNew2(UnitCommandNameRetriever, name, unit);
		}
		else
		{
			CastException((string("Bad int from :") + from).c_str());
			return 0;
		}
	}

	UnitCommandNameRetriever::~UnitCommandNameRetriever()
	{
		xDelete(_unit);
	}

	UnitCommandNameRetriever::operator int() const
	{
		return static_cast<Unit*>(*_unit)->GetCommands()->FindCommandKeyByName(_commName);
	}
}