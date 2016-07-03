#include "EcoResources.h"

namespace FlagRTS
{

	// Sets custom names etc on resources :
	// 0 ) Metal, max 99k, std max for each player = 3k, rised by building/expanding Storages
	// 1 ) Energy, max 99k, std max for each player = 1k, each generator can store limited amount
	// 2 ) 
	// 3 ) CommandPoints, max 300, for start only 1 for each player, rised by CommandCenter 
	// LogisticCenters allows to expand base

	EcoResources::EcoResources(uint8 playerCount) :
		_resourcesData(4, playerCount)
	{
		_resourcesData.SetResourceName(0, "Metal");
		_resourcesData.SetMaxAmount(0, 99999);
		_resourcesData.SetIsInvertedResource(0, false);
		_resourcesData.SetResourceName(1, "Energy");
		_resourcesData.SetMaxAmount(1, 99999);
		_resourcesData.SetIsInvertedResource(1, false);
		_resourcesData.SetResourceName(2, "Crystals");
		_resourcesData.SetMaxAmount(2, 99999);
		_resourcesData.SetIsInvertedResource(2, false);
		_resourcesData.SetResourceName(3, "Command Points");
		_resourcesData.SetMaxAmount(3, 300);
		_resourcesData.SetIsInvertedResource(3, true);
		for(uint8 i = 0; i < playerCount; ++i)
		{
			_resourcesData.SetMaxAmountForPlayer(i, 0, 3000);
			_resourcesData.SetMaxAmountForPlayer(i, 1, 10000);
			_resourcesData.SetMaxAmountForPlayer(i, 2, 1000);
			_resourcesData.SetMaxAmountForPlayer(i, 3, 10);
			_resourcesData.SetPlayerResourceAmount(i, 0, 1000);
			_resourcesData.SetPlayerResourceAmount(i, 1, 3000);
			_resourcesData.SetPlayerResourceAmount(i, 2, 1000);
			_resourcesData.SetPlayerResourceAmount(i, 3, 0);
		}
	}
}