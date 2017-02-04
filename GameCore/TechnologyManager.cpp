
#include "TechnologyManager.h"
#include "Faction.h"

namespace FlagRTS
{
	TechnologyManager::TechnologyManager(uint8 player) :
		_player(player),
		_reqsFactory(xNew0(TechRequirementFactory))
	{

	}

	TechnologyManager::~TechnologyManager()
	{
		xDelete(_reqsFactory);
		// Destroy all buildings
		for( auto buildIt = _buildings.begin(); buildIt != _buildings.end(); ++buildIt)
		{
			xDelete( *buildIt );
		}
		// Destory all reqs
		for( auto reqIt = _playerReqs.begin(); reqIt != _playerReqs.end(); ++reqIt)
		{
			xDelete( reqIt->Value );
		}
		// Destory all event delegates
		for( auto it = _reqStateChangedDelegates.begin(); it != _reqStateChangedDelegates.end(); ++it)
		{
			xDelete( *it );
		}

	}

	void TechnologyManager::SetFactionTechnology(Faction* faction)
	{
		// 1) Copy all requirements
		auto& facReqs = faction->GetTechRequirements();
		for( auto reqIt = facReqs.begin(); reqIt != facReqs.end(); ++reqIt)
		{
			auto& req = _playerReqs[reqIt->Key];
			req = reqIt->Value->GetCopy();
			req->SetPlayer(_player);
			req->SetName(reqIt->Value->GetName());
			req->SetInavailableHint(reqIt->Value->GetInavailableHint());
		}

		// 2) Copy all buildings + set reqs from this manager
		auto& facBuildings = faction->GetTechBuildings();
		_buildings.reserve(facBuildings.size());
		for( auto buildIt = facBuildings.begin(); buildIt != facBuildings.end(); ++buildIt)
		{
			// Jusr copy data, but set own reqs
			TechBuilding* newBuilding = xNew1(TechBuilding, **buildIt);
			for(unsigned int i = 0; i < newBuilding->Requirements.size(); ++i)
			{
				newBuilding->Requirements[i] = 
					_playerReqs[newBuilding->Requirements[i]->GetName()];
				// For each requirement add event handler for its state change
				newBuilding->Requirements[i]->GetStateChangedEvent() +=
					xNew2(TechRequirementForBuildingStateChangedDelegate, this, newBuilding);
			}
			_buildings.push_back(newBuilding);
		}
	}

	void TechnologyManager::LoadMapSpecificTechnology(XmlNode* mapTechsNode)
	{
		// TODO

	}

	void TechnologyManager::CheckAllRequirements()
	{
		for(unsigned int i = 0; i < _buildings.size(); ++i)
		{
			for(unsigned int req = 0; req < _buildings[i]->Requirements.size(); ++req)
			{
				_buildings[i]->Requirements[req]->CheckRequirementQuiet();
			}

			TechAvailability oldAv = _buildings[i]->Availability;
			TechAvailability newAv = GetFinalAvailability(
				_buildings[i]->Requirements);
			if( oldAv != newAv )
			{
				_buildings[i]->Availability = newAv;
				BuilidngAvailabilityChanged().Fire(_buildings[i], GetPlayer());
			}
		}
	}

	GlobalTechnologyManager::GlobalTechnologyManager(uint8 playersCount)
	{
		_techMgrs.resize(playersCount);
		for(unsigned int i = 0; i < playersCount; ++i)
		{
			_techMgrs[i] = xNew1(TechnologyManager, i);
		}
	}

	GlobalTechnologyManager::~GlobalTechnologyManager()
	{
		for(unsigned int i = 0; i < _techMgrs.size(); ++i)
		{
			xDelete(_techMgrs[i]);
		}
	}
}