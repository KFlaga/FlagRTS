#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
	class GameSettings
	{
	public:
		// Speed of the game -> determines how fast game is running.
		// Virtual in-game update interval equals this * real_interval
		float GameSpeed; 
		// Max update speed. If != 0 game tries to update with fixed rate
		int MaxFPS;
	
		// Sets default values
		GameSettings() :
			GameSpeed(1.0f),
			MaxFPS(60)
		{ }
	};
}