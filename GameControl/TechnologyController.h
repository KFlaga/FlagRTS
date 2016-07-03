#pragma once

#include <DataTypes.h>
#include <Xml.h>
#include <Array.h>
#include <Event.h>
#include <Technology.h>
#include <HashMap.h>

namespace FlagRTS
{
	class GlobalTechnologyManager;
	class TechnologyManager;
	// class RequirementsTracker;

	// Tracks technology availablility from player TechnologyManager
	// and pass it to other controllers
	class TechnologyController
	{
	private:
		GlobalTechnologyManager* _techsMgr;
		TechnologyManager* _clientTechMgr;
		// RequirementsTracker* _reqsTracker; // RequirementsTracker periodicaly checks if requirements are filled ( on other thread ) 
		// size_t _reqsTrackerThreadHandle;
		uint8 _cplayer;

		Event<TechBuilding*, uint8> _builidngAvailabilityChanged;

	public:
		TechnologyController(GlobalTechnologyManager* techMgr, uint8 clientPlayer);
		~TechnologyController();

		// Starts RequirementsTracker thread
		//void BeginRequirementTracking();
		// Ends RequirementsTracker thread
		//void EndRequirementTracking();

		void Update(float ms);

		// Checks all requirements for all technology for all players
		void CheckAllRequirements();

		// Returns all buildings for given player
		const Array<TechBuilding*>& GetAllBuilidings(uint8 player) const;

		// Fills array with all not hidden buildings ( so they can be shown to player etc. )
		void GetNotHiddenBuildings(uint8 player, Array<TechBuilding*>& targetArray);

		// Event occurs when some building availability changes, probably due to change in
		// its reqirements. Returns updated TechBuilding and player update is about
		Event<TechBuilding*, uint8>& BuilidngAvailabilityChanged(uint8 player);
	};
}