#include "FactionRetreivers.h"
#include <Exception.h>
#include "MainEventPool.h"
#include "Player.h"
#include "OwnerRetrievers.h"
#include "GameWorld.h"

namespace FlagRTS
{
	IObjectRetriever<string>* FactionRetrieverFactory::Create(XmlNode* valueNode)
	{
		// Find retriever type
		const char* from = XmlUtility::XmlGetString(valueNode, "from", 4);

		if(strcmp(from, "Value") == 0)
		{
			string val = XmlUtility::XmlGetString(valueNode, "value");
			return xNew1(FactionFixedRetriever, val);
		}
		else if(strcmp(from, "Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(valueNode, "value");
			return xNew1(FactionVariableRetriever, varName);
		}
		else if(strcmp(from, "Player") == 0)
		{
			XmlNode* playerNode = valueNode->first_node("Player");
			auto player = OwnerRetrieverFactory::Create(playerNode);
			return xNew1(FactionOfPlayerRetriever, player);
		}
		else
		{
			CastException((string("Bad faction from :") + from).c_str());
			return 0;
		}
	}

	FactionVariableRetriever::operator string() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<string>();
	}

	FactionOfPlayerRetriever::~FactionOfPlayerRetriever()
	{
		xDelete(_player);
	}

	FactionOfPlayerRetriever::operator string() const
	{
		return GameWorld::GlobalWorld->GetPlayers()->GetPlayer(
			*_player)->GetFaction();
	}
}