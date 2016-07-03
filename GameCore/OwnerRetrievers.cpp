#include "OwnerRetrievers.h"
#include "GameWorld.h"
#include "Player.h"
#include "MainEventPool.h"

namespace FlagRTS
{

	IObjectRetriever<int>* OwnerRetrieverFactory::Create(XmlNode* ownerNode)
	{
		string from = XmlUtility::XmlGetString(ownerNode, "from", 4);
		if(from.compare("Value") == 0)
		{
			return xNew1(OwnerXmlNumberRetriever, ownerNode);
		}
		else if(from.compare("Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(ownerNode, "value");
			return xNew1(OwnerVariableRetriever, varName);
		}
		else if(from.compare("ClientPlayer") == 0)
		{
			return xNew0(OwnerClientPlayerRetriever);
		}
		return 0;
	}

	OwnerVariableRetriever::operator int() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<int>();
	}

	OwnerClientPlayerRetriever::operator int() const
	{
		return GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
	}
}