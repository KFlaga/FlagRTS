#include "Resources.h"
#include <Array.h>
#include <StringUtils.h>
#include "UnitStats.h"
#include "InGameSettings.h"
#include "GameWorld.h"
#include "Player.h"


#define INVALID_RESOURCE (uint8)-1

namespace FlagRTS
{
	struct ResourceInfo
	{
		string Name;
		int AbsoluteMaxAmount;
		bool IsInverted;
		uint8 Number;
	};

	struct PlayerResources
	{
		int Amount;
		int MaxAmount;
	};

	struct ResourcesInternal
	{
	public:
		Array<ResourceInfo> Resources;
		Array<PlayerResources> Players;

		ResourcesInternal(uint8 resourcesCount, uint8 playerCount)
		{
			Resources.resize(resourcesCount);
			for(unsigned int i = 0; i < Resources.size(); ++i)
			{
				Resources[i].Number = i;
				Resources[i].Name = "";
				Resources[i].IsInverted = false;
			}

			Players.resize(playerCount*resourcesCount);
		}

		~ResourcesInternal()
		{

		}

		PlayerResources& GetPlayerResource(const uint8 playerNum, const uint8 resNum)
		{
			return Players[playerNum * Resources.size() + resNum];
		}
	};

	Resources::Resources(uint8 resourcesCount, uint8 playerCount) 
	{
		_internal = xNew2(ResourcesInternal, resourcesCount, playerCount);
	}

	Resources::Resources() 
	{
		_internal = xNew2(ResourcesInternal, 3, 9);
		SetDefaultValues();
	}

	Resources::~Resources()
	{
		xDeleteSafe(_internal);
	}

	void Resources::SetResourcesSettings(InGameSettings* settings)
	{
		int playerCount = _internal->Players.size() / _internal->Resources.size();
		for(unsigned int i = 0; i < settings->ResourceSettings.size(); ++i)
		{
			uint8 num = GetResourceNumber(settings->ResourceSettings[i].ResourceName);
			if(num != INVALID_RESOURCE)
			{
				SetMaxAmount(num, settings->ResourceSettings[i].AbsoluteMaxAmount);
				for(unsigned int pl = 0; pl < playerCount; ++pl)
				{
					SetMaxAmountForPlayer(pl, num, settings->ResourceSettings[i].StartMaxAmount);
					SetPlayerResourceAmount(pl, num, settings->ResourceSettings[i].StartAmount);
				}
			}
		}
	}

	const string& Resources::GetResourceName(const uint8 resource)
	{
		return _internal->Resources[resource].Name;
	}

	void Resources::SetResourceName(const uint8 resource, const char* name)
	{
		_internal->Resources[resource].Name = name;
	}

	void Resources::SetResourceName(const uint8 resource, const string& name)
	{
		_internal->Resources[resource].Name = name;
	}

	uint8 Resources::GetResourceNumber(const string& resource)
	{
		for(auto res = _internal->Resources.begin(), end =  _internal->Resources.end(); 
			res != end; ++res)
		{
			if(resource.compare(res->Name) == 0)
			{
				return res->Number;
			}
		}
		return INVALID_RESOURCE;
	}

	uint8 Resources::GetResourceNumber(const char* resource)
	{
		for(auto res = _internal->Resources.begin(), end =  _internal->Resources.end(); 
			res != end; ++res)
		{
			if(strcmp(resource,res->Name.c_str()) == 0)
			{
				return res->Number;
			}
		}
		return INVALID_RESOURCE;
	}

	uint8 Resources::GetResourcesCount()
	{
		return _internal->Resources.size();
	}

	bool Resources::IsInvertedResource(const uint8 resource)
	{
		return _internal->Resources[resource].IsInverted;
	}

	void Resources::SetIsInvertedResource(const uint8 resource, bool isInv)
	{
		_internal->Resources[resource].IsInverted = isInv;
	}

	void Resources::SetMaxAmount(const uint8 resource, const int maxAmount)
	{
		_internal->Resources[resource].AbsoluteMaxAmount = maxAmount;
	}

	int Resources::GetMaxAmount(const uint8 resource)
	{
		return _internal->Resources[resource].AbsoluteMaxAmount;
	}

	int Resources::GetPlayerResourceAmount(const uint8 playerNum, const uint8 resNum)
	{
		return _internal->GetPlayerResource(playerNum, resNum).Amount;
	}

	void Resources::SetPlayerResourceAmount(const uint8 playerNum, const uint8 resNum, const int amount)
	{
		_internal->GetPlayerResource(playerNum, resNum).Amount = amount;
	}

	void Resources::AddResourceToPlayer(const uint8 playerNum, const uint8 resNum, const int amount)
	{
		auto& res = _internal->GetPlayerResource(playerNum, resNum);
		if( IsInvertedResource(resNum ) )
		{
			res.Amount = std::max(res.Amount - amount, 0);
		}
		else
		{
			res.Amount = std::min(res.Amount + amount, res.MaxAmount);
		}
	}

	void Resources::TakeResourceFromPlayer(const uint8 playerNum, const uint8 resNum, const int amount)
	{
		auto& res = _internal->GetPlayerResource(playerNum, resNum);
		if( IsInvertedResource(resNum ) )
		{
			res.Amount = std::min(res.Amount + amount, res.MaxAmount);
		}
		else
		{
			res.Amount = std::max(res.Amount - amount, 0);
		}
	}

	void Resources::SetMaxAmountForPlayer(const uint8 playerNum, 
		const uint8 resNum, 
		const int maxAmount)
	{
		_internal->GetPlayerResource(playerNum, resNum).MaxAmount = maxAmount;
	}

	int Resources::GetMaxAmountForPlayer(const uint8 playerNum, 
		const uint8 resNum)
	{
		return _internal->GetPlayerResource(playerNum, resNum).MaxAmount;
	}

	bool Resources::CheckPlayerHaveSufficientResources(const uint8 playerNum, 
		const UnitCost& unitCost, uint8* insufficientResourceNum)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			if( IsInvertedResource(resNum) )
			{
				if( GetPlayerResourceAmount(playerNum, resNum) + unitCost[i].ResAmount >
					GetMaxAmountForPlayer(playerNum, resNum) )
				{
					*insufficientResourceNum = resNum;
					return false;
				}
			}
			else if( GetPlayerResourceAmount(playerNum, resNum) < unitCost[i].ResAmount )
			{
				*insufficientResourceNum = resNum;
				return false;
			}
		}
		return true;
	}

	void Resources::TakeResourcesFromPlayer(const uint8 playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			TakeResourceFromPlayer(playerNum, resNum, unitCost[i].ResAmount); 
		}
	}

	void Resources::ReturnResourcesOnDeathToPlayer(const uint8 playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			if( unitCost[i].ReturnOnDeath )
			{
				auto resNum = GetResourceNumber(unitCost[i].ResName);
				AddResourceToPlayer(playerNum, resNum, unitCost[i].ResAmount); 
			}
		}
	}

	void Resources::ReturnAllResourcesToPlayer(const uint8 playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			AddResourceToPlayer(playerNum, resNum, unitCost[i].ResAmount);
		}
	}

	void Resources::SetDefaultValues()
	{
		auto& resources = _internal->Resources;

		resources[0].Number = 0;
		resources[0].AbsoluteMaxAmount = 200;
		resources[0].IsInverted = false;
		resources[0].Name = "Funds";

	}
}