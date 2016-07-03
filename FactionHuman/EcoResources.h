#include <Resources.h>

namespace FlagRTS
{
	// Sets custom names etc on resources :
	// 0 ) Metal, max 99k, std max for each player = 3k, rised by building/expanding Storages
	// 1 ) Energy, max 99k, std max for each player = 1k, each generator can store limited amount
	// 2 ) Crystals
	// 3 ) CommandPoints, max 100, for start only 1 for each player, rised by CommandCenter 
	// LogisticCenters allows to expand base
	class EcoResources
	{
		Resources _resourcesData;

	public:
		EcoResources(uint8 playerCount);
		Resources* GetResourcesData() { return &_resourcesData; }

		enum Resources
		{
			Metal = 0,
			Energy = 1,
			Crystals = 2,
			CommandPoint = 3
		};
	};
}