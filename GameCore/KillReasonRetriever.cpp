#include "KillReasonRetriever.h"
#include "IntRetrievers.h"
#include <Exception.h>
#include "KillReason.h"
#include "OwnerRetrievers.h"

namespace FlagRTS
{
	IObjectRetriever<int>* KillReasonRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			int val = XmlUtility::XmlGetInt(valueNode, "value", 5);
			return xNew1(IntFixedRetriever, val);
		}
		else if(strcmp(from, "Name") == 0)
		{
			const char* name = XmlUtility::XmlGetString(valueNode, "name", 5);

			return xNew1(KillReasonFixedRetriever, KillReasons::ParseKillReason(name));
		}
		else if(strcmp(from, "KilledByPlayer") == 0)
		{
			IObjectRetriever<int>* player = OwnerRetrieverFactory::Create(
				valueNode->first_node("Player"));

			return xNew1(KillReasonKilledByPlayerRetriever, player);
		}
		else
		{
			CastException((string("Bad from :") + from).c_str());
			return 0;
		}
	}

	KillReasonKilledByPlayerRetriever::~KillReasonKilledByPlayerRetriever()
	{
		xDelete(_player);
	}

	KillReasonKilledByPlayerRetriever::operator int() const
	{
		return KillReasons::GetKilledByPlayerReason(*_player);
	}
}