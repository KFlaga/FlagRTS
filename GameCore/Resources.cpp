#include "Resources.h"
#include <Array.h>
#include <StringUtils.h>
#include "UnitStats.h"
#include "InGameSettings.h"
#include "GameWorld.h"
#include "Player.h"

namespace FlagRTS
{
	Resources::Resources(int resourcesCount, int playerCount) 
	{
		_definitions.resize(resourcesCount);

		for(int i = 0; i < resourcesCount; ++i)
		{
			_definitions[i].Index = i;
			_definitions[i].Name = "";
			_definitions[i].IsSupply = false;
		}

		_playerResources.resize(playerCount);
		for(int i = 0; i < playerCount; ++i)
		{
			_playerResources.resize(resourcesCount);
		}
	}

	void Resources::SetResourcesSettings(InGameSettings* settings)
	{
		for(int i = 0; i < settings->ResourceSettings.size(); ++i)
		{
			int num = GetResourceNumber(settings->ResourceSettings[i].ResourceName);
			if(num != INVALID_RESOURCE)
			{
				SetMaxAmount(num, settings->ResourceSettings[i].AbsoluteMaxAmount);
				for(int pl = 0; pl < _playerResources.size(); ++pl)
				{
					SetMaxAmountForPlayer(pl, num, settings->ResourceSettings[i].StartMaxAmount);
					SetPlayerResourceAmount(pl, num, settings->ResourceSettings[i].StartAmount);
				}
			}
		}
	}

	const string& Resources::GetResourceName(const int resource)
	{
		return _definitions[resource].Name;
	}

	void Resources::SetResourceName(const int resource, const char* name)
	{
		_definitions[resource].Name = name;
	}

	void Resources::SetResourceName(const int resource, const string& name)
	{
		_definitions[resource].Name = name;
	}

	int Resources::GetResourceNumber(const string& resource)
	{
		for(int i = 0; i < _definitions.size(); ++i)
		{
			if(resource.compare(_definitions[i].Name) == 0)
			{
				return _definitions[i].Index;
			}
		}
		return INVALID_RESOURCE;
	}

	int Resources::GetResourceNumber(const char* resource)
	{
		for(int i = 0; i < _definitions.size(); ++i)
		{
			if(strcmp(resource, _definitions[i].Name.c_str()) == 0)
			{
				return _definitions[i].Index;
			}
		}
		return INVALID_RESOURCE;
	}

	int Resources::GetResourcesCount()
	{
		return _definitions.size();
	}

	bool Resources::IsSupplyResource(const int resource)
	{
		return _definitions[resource].IsSupply;
	}

	void Resources::SetIsSupplyResource(const int resource, bool isSupply)
	{
		_definitions[resource].IsSupply = isSupply;
	}

	void Resources::SetMaxAmount(const int resource, const float maxAmount)
	{
		_definitions[resource].AbsoluteMaxAmount = maxAmount;
	}

	float Resources::GetMaxAmount(const int resource)
	{
		return _definitions[resource].AbsoluteMaxAmount;
	}

	float Resources::GetPlayerResourceAmount(const int playerNum, const int resNum)
	{
		return GetPlayerResource(playerNum, resNum).Amount;
	}

	void Resources::SetPlayerResourceAmount(const int playerNum, const int resNum, const float amount)
	{
		GetPlayerResource(playerNum, resNum).Amount = amount;
	}

	void Resources::AddResourceToPlayer(const int playerNum, const int resNum, const float amount)
	{
		auto& res = GetPlayerResource(playerNum, resNum);
		res.Amount = std::min(res.Amount + amount, res.MaxAmount);
	}

	void Resources::TakeResourceFromPlayer(const int playerNum, const int resNum, const float amount)
	{
		auto& res = GetPlayerResource(playerNum, resNum);
		res.Amount = std::max(res.Amount - amount, 0.f);
	}

	void Resources::SetMaxAmountForPlayer(const int playerNum, 
		const int resNum, 
		const float maxAmount)
	{
		GetPlayerResource(playerNum, resNum).MaxAmount = maxAmount;
	}

	float Resources::GetMaxAmountForPlayer(const int playerNum, 
		const int resNum)
	{
		return GetPlayerResource(playerNum, resNum).MaxAmount;
	}

	bool Resources::CheckPlayerHaveSufficientResources(const int playerNum, 
		const UnitCost& unitCost, int* insufficientResourceNum)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			if( GetPlayerResourceAmount(playerNum, resNum) < unitCost[i].ResAmount )
			{
				*insufficientResourceNum = resNum;
				return false;
			}
		}
		return true;
	}

	void Resources::TakeResourcesFromPlayer(const int playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			TakeResourceFromPlayer(playerNum, resNum, unitCost[i].ResAmount); 
		}
	}

	void Resources::ReturnResourcesOnDeathToPlayer(const int playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			if( _definitions[i].IsSupply )
			{
				auto resNum = GetResourceNumber(unitCost[i].ResName);
				AddResourceToPlayer(playerNum, resNum, unitCost[i].ResAmount); 
			}
		}
	}

	void Resources::ReturnAllResourcesToPlayer(const int playerNum, const UnitCost& unitCost)
	{
		for(unsigned int i = 0; i < unitCost.size(); ++i)
		{
			auto resNum = GetResourceNumber(unitCost[i].ResName);
			AddResourceToPlayer(playerNum, resNum, unitCost[i].ResAmount);
		}
	}
}