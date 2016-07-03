#include "TargetValidator.h"
#include "Unit.h"
#include "MapDecorator.h"
#include "Player.h"

namespace ORTS
{
	HoverTargetValidator::CheckMap* HoverTargetValidator::_flagChecksMap = 0;

	HoverTargetValidator::HoverTargetValidator()
	{

	}

	HoverTargetValidator::HoverTargetValidator(XmlNode* targetNode)
	{
		// First find ObjectType node and "value" attribute with expected type
		// There are few possibilities for types :
		// - None -> passed pointer is invalid
		// - Any -> no check
		// - SpecificType -> check TypeId of FinalType
		string type = targetNode->first_node("ObjectType", 10)->first_attribute()->value();
		if(type.compare("None") == 0)
		{
			_checks.push_back( std::make_pair(false,
				[](RefPtr<PhysicalObject>& obj) { 
				return obj.IsValid(); } ));
		}
		else if(type.compare("Unit") == 0)
		{
			_checks.push_back( std::make_pair(true,
				[](RefPtr<PhysicalObject>& obj) { 
				return obj->GetFinalType() == GetTypeId<Unit>(); } ));
		}
		else if(type.compare("MapDecorator") == 0)
		{
			_checks.push_back( std::make_pair(true,
				[](RefPtr<PhysicalObject>& obj) { 
				return obj->GetFinalType() == GetTypeId<MapDecorator>(); } ));
		}
		// Find Flags node and for every node
		// build check with proper flag
		XmlNode* flagsNode = targetNode->first_node("Flags",5);
		if(flagsNode != 0)
		{
			XmlNode* flagNode = flagsNode->first_node();
			while(flagNode != 0)
			{
				// Find flag check with node name as key and add to checks
				auto checkIt = _flagChecksMap->find(flagNode->first_attribute()->value());
				if(checkIt != _flagChecksMap->end())
				{
					AddCheck(XmlUtility::XmlGetBool(flagNode, "value", 5), 
						checkIt->second);
				}

				flagNode = flagNode->next_sibling();
			}
		}
	}

#define AddFlagCheck(name, check) _flagChecksMap->insert(std::make_pair( \
	name, \
	[](RefPtr<PhysicalObject>& obj)->bool {\
		return check; \
	}));

	void HoverTargetValidator::InitCheckMap()
	{
		_flagChecksMap = xNew0(CheckMap);
		AddFlagCheck("IsGroundPassable", 
			obj->IsGroundPassable() == true);
		AddFlagCheck("IsAirPassable", 
			obj->IsAirPassable() == true);
		AddFlagCheck("IsOwned", 
			PlayersInfo::CurrentPlayers->GetClientPlayer() == 
			reinterpret_cast<RefPtr<Unit>&>(obj)->GetOwner()); 
		AddFlagCheck("IsAlly", 
			PlayersInfo::CurrentPlayers->IsAlly(
			PlayersInfo::CurrentPlayers->GetClientPlayer(), 
			reinterpret_cast<RefPtr<Unit>&>(obj)->GetOwner())); 
		AddFlagCheck("IsNeutral", 
			PlayersInfo::CurrentPlayers->IsNeutral(
			PlayersInfo::CurrentPlayers->GetClientPlayer(), 
			reinterpret_cast<RefPtr<Unit>&>(obj)->GetOwner()));
		AddFlagCheck("IsEnemy", 
			PlayersInfo::CurrentPlayers->IsEnemy(
			PlayersInfo::CurrentPlayers->GetClientPlayer(), 
			reinterpret_cast<RefPtr<Unit>&>(obj)->GetOwner()));
		AddFlagCheck("CanGarnizon", 
			reinterpret_cast<RefPtr<Unit>&>(obj)->CanGarnizon());
	}

	void HoverTargetValidator::DeleteCheckMap()
	{
		xDeleteSafe(_flagChecksMap);
	}
}