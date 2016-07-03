#pragma once

#include "DataTypes.h"
#include <Array.h>

namespace FlagRTS
{
	struct ResourcesInternal;
	struct ResourceCost;
	typedef Array<ResourceCost> UnitCost; // Cost of unit i'th position in array is cost of i'th resource
	struct InGameSettings;

	// Definition and current amounts of used resources
	class Resources
	{
		ResourcesInternal* _internal;

	public:
		// Sets default resources info
		Resources();
		Resources(uint8 resourcesCount, uint8 playerCount);
		~Resources();

		void SetResourcesSettings(InGameSettings* settings);

		const string& GetResourceName(const uint8 resource);
		void SetResourceName(const uint8 resource, const string& name);
		void SetResourceName(const uint8 resource, const char* name);
		uint8 GetResourceNumber(const string& resource);
		uint8 GetResourceNumber(const char*  resource);
		uint8 GetResourcesCount();

		// Inverted resource means that player can use some max amount of this resource
		// like max people count
		bool IsInvertedResource(const uint8 resource);
		void SetIsInvertedResource(const uint8 resource, const bool isInv);

		void SetMaxAmount(const uint8 resNum, const int maxAmount);
		int GetMaxAmount(const uint8 resNum);

		int GetPlayerResourceAmount(const uint8 playerNum, const uint8 resNum);
		void SetPlayerResourceAmount(const uint8 playerNum, const uint8 resNum, const int amount);
		void AddResourceToPlayer(const uint8 playerNum, const uint8 resNum, const int amount);
		void TakeResourceFromPlayer(const uint8 playerNum, const uint8 resNum, const int amount);

		void SetMaxAmountForPlayer(const uint8 playerNum, 
			const uint8 resNum, 
			const int maxAmount);
		
		int GetMaxAmountForPlayer(const uint8 playerNum, 
			const uint8 resNum);

		// Returns true if player have all neccessary resources
		// If have not, returns false + in insufficientResourceNum sets
		// first resource player lacks
		bool CheckPlayerHaveSufficientResources(const uint8 playerNum, 
			const UnitCost& unitCost, uint8* insufficientResourceNum);

		// Takes all resources from array from player
		void TakeResourcesFromPlayer(const uint8 playerNum, const UnitCost& unitCost);

		// Returns all resources marked 'return on death' to player
		void ReturnResourcesOnDeathToPlayer(const uint8 playerNum, const UnitCost& unitCost);

		// Returns all resources from cost to player
		void ReturnAllResourcesToPlayer(const uint8 playerNum, const UnitCost& unitCost);

	private:
		void SetDefaultValues();
	};

	struct ResourceSetting
	{
		string ResourceName;
		int StartAmount;
		int StartMaxAmount;
		int AbsoluteMaxAmount;
	};
}