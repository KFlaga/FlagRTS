#pragma once

#include <DllPlugin.h>

namespace FlagRTS
{
	class GameWorld;
	typedef void(*FuncPtr)();

	// Game mods should register unit definitions / suppliers etc to GameWorld
	class DllApi DllGameMod : public DllPlugin
	{
	protected:
		GameWorld* _gameWorld;

	public:
		DllGameMod() :
			DllPlugin(),
			_gameWorld(0)
		{ }

		// Sets game world - must be called prior to Init
		void SetGameWorld(GameWorld* world) { _gameWorld = world; }
		GameWorld* GetGameWorld() const { return _gameWorld; }
	};
}