#pragma once

#include <TypeDefs.h>


namespace FlagRTS
{
	/*
		Interface for states of entire application : if its in loading/menu/game screen
		Switching between states via app state machine
	*/
	class IAppState 
	{
	public:
		IAppState() { }
		virtual ~IAppState() { }
		// Request necessary resources to be loaded
		// To be called prior to 1st start
		virtual ErrorCode Load() = 0;
		// Frees resources previously loaded
		// To be called when state is not likely to be used
		virtual ErrorCode Unload() = 0;
		// Starts the state, that is renders specific screen and captures input
		virtual ErrorCode Start() = 0;
		// Ends state - cleans what necessary and frees input
		virtual ErrorCode End() = 0;
		// Updates game logic every frame
		virtual ErrorCode Update(float ms) = 0;
	};
}