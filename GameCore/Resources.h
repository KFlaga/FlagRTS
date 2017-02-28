#pragma once

#include "DataTypes.h"
#include <Array.h>

#define INVALID_RESOURCE -1

namespace FlagRTS
{
	struct ResourcesInternal;
	struct ResourceCost;
	typedef Array<ResourceCost> UnitCost; // Cost of unit i'th position in array is cost of i'th resource
	struct InGameSettings;

	// Definition and current amounts of used resources
	/**
	Stores definitions of in-game resources and their amounts for each player.
	*/
	class Resources
	{
	public:
		struct ResourceDefinition
		{
			string Name;
			int Index;
			float AbsoluteMaxAmount;
			bool IsSupply;
		};

		struct PlayerResource
		{
			float Amount;
			float MaxAmount;
		};

	protected:
		Array<ResourceDefinition> _definitions;
		Array<Array<PlayerResource>> _playerResources; // [player][resIdx] -> PlayerResource

	public:
		// Sets default resources info
		Resources(int resourcesCount, int playerCount);

		void SetResourcesSettings(InGameSettings* settings);

		const string& GetResourceName(const int resource);
		void SetResourceName(const int resource, const string& name);
		void SetResourceName(const int resource, const char* name);
		int GetResourceNumber(const string& resource);
		int GetResourceNumber(const char*  resource);
		int GetResourcesCount();

		bool IsSupplyResource(const int resource);
		void SetIsSupplyResource(const int resource, const bool isSupply);

		void SetMaxAmount(const int resNum, const float maxAmount);
		float GetMaxAmount(const int resNum);

		float GetPlayerResourceAmount(const int playerNum, const int resNum);
		void SetPlayerResourceAmount(const int playerNum, const int resNum, const float amount);
		void AddResourceToPlayer(const int playerNum, const int resNum, const float amount);
		void TakeResourceFromPlayer(const int playerNum, const int resNum, const float amount);

		void SetMaxAmountForPlayer(const int playerNum, 
			const int resNum, 
			const float maxAmount);

		float GetMaxAmountForPlayer(const int playerNum, 
			const int resNum);

		// Returns true if player have all neccessary resources
		// If have not, returns false + in insufficientResourceNum sets
		// first resource player lacks
		bool CheckPlayerHaveSufficientResources(const int playerNum, 
			const UnitCost& unitCost, int* insufficientResourceNum);

		// Takes all resources from array from player
		void TakeResourcesFromPlayer(const int playerNum, const UnitCost& unitCost);

		// Returns all resources marked 'supply' to player
		void ReturnResourcesOnDeathToPlayer(const int playerNum, const UnitCost& unitCost);

		// Returns all resources from cost to player
		void ReturnAllResourcesToPlayer(const int playerNum, const UnitCost& unitCost);
		
		PlayerResource& GetPlayerResource(const int playerNum, const int resNum)
		{
			return _playerResources[playerNum][resNum];
		}
	};

	struct ResourceSetting
	{
		string ResourceName;
		int StartAmount;
		int StartMaxAmount;
		int AbsoluteMaxAmount;
	};
}