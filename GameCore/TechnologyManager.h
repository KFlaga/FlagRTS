#pragma once

#include "TechRequirement.h"
#include "DataTypes.h"
#include <HashMap.h>
#include <Event.h>
#include <Xml.h>

namespace FlagRTS
{
	class Faction;

	// Contains all available technology ( buildings, researches )
	// for player + tracks its progress, also contains player specific TechReqs
	// Copies technology from faction defaults, but every tech may be overriden / added
	// in map-specific file 
	class TechnologyManager
	{
		uint8 _player;

		// Maps tech requirements with their names
		// Reqs are taken from faction technology + loaded from map-tech file
		HashedMaps<TechRequirement*>::KeyCCString _playerReqs;
		TechRequirementFactory* _reqsFactory;

		class TechRequirementForBuildingStateChangedDelegate;
		Array<TechRequirementForBuildingStateChangedDelegate*> _reqStateChangedDelegates;

		// Contains all building possible for construction on this map
		// and their current availability and requierments
		Array<TechBuilding*> _buildings;
		Event<TechBuilding*, uint8> _builidngAvailabilityChanged;
		
	public:
		TechnologyManager(uint8 player);
		~TechnologyManager();

		uint8 GetPlayer() const { return _player; }

		void SetFactionTechnology(Faction* faction);

		void LoadMapSpecificTechnology(XmlNode* mapTechsNode);

		// Checks all requirements for all technology
		// Fires AvailabilityChanged events if some requirements are filled ( or opposite )
		// which leaded to change in some technology availability
		void CheckAllRequirements();
		
		const Array<TechBuilding*>& GetAllBuilidings() const
		{
			return _buildings;
		}
		// Event occurs when some building availability changes, probably due to change in
		// its reqirements. Returns updated TechBuilding and player update is about
		Event<TechBuilding*, uint8>& BuilidngAvailabilityChanged() 
		{ 
			return _builidngAvailabilityChanged; 
		}

		// Returns TechRequiremrnt with given name or 0 if not found
		TechRequirement* FindRequirement(const char* name)
		{
			auto reqIt = _playerReqs.find(name);
			if(reqIt != _playerReqs.end())
				return reqIt->Value;
			return 0;
		}

	private:
		// Event handler for change in some building requirement change :
		// if req state changed find availability for building again and
		// if changed, fire event
		class TechRequirementForBuildingStateChangedDelegate : public EventHandler<TechRequirement*>
		{
			TechnologyManager* _owner;
			TechBuilding* _building;

		public:
			TechRequirementForBuildingStateChangedDelegate(TechnologyManager* owner, TechBuilding* building) :
				_owner(owner),
				_building(building)
			{ }

			void operator()(TechRequirement* req)
			{
				TechAvailability oldAv = _building->Availability;
				TechAvailability newAv = GetFinalAvailability(_building->Requirements);
				if( oldAv != newAv )
				{
					_building->Availability = newAv;
					_owner->BuilidngAvailabilityChanged().Fire(
						_building, _owner->GetPlayer());
				}
			}
		};
	};

	// Contains technology managers for all players
	class GlobalTechnologyManager
	{
		Array<TechnologyManager*> _techMgrs;

	public:
		GlobalTechnologyManager(uint8 playersCount);
		~GlobalTechnologyManager();

		TechnologyManager* GetPlayerTechs(uint8 player)
		{
			return _techMgrs[player];
		}

		unsigned int GetPlayerTechsCount()
		{
			return _techMgrs.size();
		}
	};
}