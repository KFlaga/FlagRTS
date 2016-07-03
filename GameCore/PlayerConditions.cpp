
#include "PlayerConditions.h"
#include "Player.h"
#include "GameWorld.h"
#include "MainEventPool.h"
#include "OwnerRetrievers.h"
#include "FactionRetreivers.h"

namespace FlagRTS
{
	PlayerFamilyConditionFactory::PlayerFamilyConditionFactory()
	{
		_factories["IsInGame"] = xNew0(PlayerIsInGameCondition::Factory);
		_factories["HaveFaction"] = xNew0(PlayerHaveFactionCondition::Factory);
	}

	PlayerFamilyConditionFactory::~PlayerFamilyConditionFactory()
	{
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
	}

	ICondition* PlayerFamilyConditionFactory::Create(XmlNode* condNode)
	{
		ICondition* condition = 0;
		string typeName = XmlUtility::XmlGetString(condNode, "type");
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			if( facIt->first.compare(typeName) == 0 )
			{
				condition = facIt->second->Create(condNode);
			}
		}
		return condition;
	}

	bool PlayerIsInGameCondition::CheckCondition()
	{
		return GameWorld::GlobalWorld->GetPlayers()->GetPlayer(*_player)->IsUsed();
	}

	PlayerIsInGameCondition* PlayerIsInGameCondition::GetCopy()
	{
		PlayerIsInGameCondition* cond = xNew0(PlayerIsInGameCondition);
		cond->_player = _player;
		return cond;
	}

	ICondition* PlayerIsInGameCondition::Factory::Create(XmlNode* condNode)
	{
		PlayerIsInGameCondition* newCond = xNew0(PlayerIsInGameCondition);

		// Get first node -> it should be Condition
		XmlNode* playerNode = condNode->first_node("Player");
		IObjectRetriever<int>* player = OwnerRetrieverFactory::Create(playerNode);
		newCond->SetParameters(player);

		return newCond;
	}

	bool PlayerHaveFactionCondition::CheckCondition()
	{
		return GameWorld::GlobalWorld->GetPlayers()->GetPlayer(*_player)->
			GetFaction().compare(*_faction) == 0;
	}

	PlayerHaveFactionCondition* PlayerHaveFactionCondition::GetCopy()
	{
		PlayerHaveFactionCondition* cond = xNew0(PlayerHaveFactionCondition);
		cond->_faction = _faction;
		cond->_player = _player;
		return cond;
	}

	ICondition* PlayerHaveFactionCondition::Factory::Create(XmlNode* condNode)
	{
		PlayerHaveFactionCondition* newCond = xNew0(PlayerHaveFactionCondition);

		// Get first node -> it should be Condition
		XmlNode* playerNode = condNode->first_node("Player");
		IObjectRetriever<int>* player = OwnerRetrieverFactory::Create(playerNode);
		
		XmlNode* factionNode = condNode->first_node("Faction");
		IObjectRetriever<string>* faction = FactionRetrieverFactory::Create(factionNode);
		newCond->SetParameters(player, faction);

		return newCond;
	}
}