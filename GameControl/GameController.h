#pragma once

#include <Ogre.h>
#include "DataTypes.h"
#include "InGameSettings.h"
#include <Array.h>
#include <Xml.h>

namespace FlagRTS
{
	class CameraController;
	class GameWorld;
	class MainGameObjectPool;
	class MainEventPool;
	class InteractionController;
	class SelectionController;
	class GuiController;
	class PathingSystem;
	class DllGameMod;
	class TechnologyController;
	class GlobalStatisticsManager;
	class GlobalTechnologyManager;
	class Console;
	class ConsoleBrigde;
	class DllGameMod;

	// Main in-game class, responsible for initializing game, 
	// creating game map, store and update all game components,
	// process messages ( or at least delegate those tasks to other
	// objects ). Basicly creates, contorol and end the game.
	class GameController
	{
	private:
		Ogre::Root* _ogreRoot;
		Ogre::RenderWindow* _window;
		Ogre::SceneManager* _ogreSceneMgr;
		GameWorld* _GameWorld;
		MainGameObjectPool* _objectPool;
		MainEventPool* _eventPool;
		CameraController* _camController;
		InteractionController* _inputController;
		SelectionController* _selectionController;
		InGameSettings _gameSettings;
		GuiController* _guiController;
		TechnologyController* _techController;
		PathingSystem* _pathingSystem;
		GlobalStatisticsManager* _statsManager;
		GlobalTechnologyManager* _techsManager;
		Array<DllGameMod*> _loadedMods;
		Console* _console;
		ConsoleBrigde* _consoleBridge;

	public:
		GameController(Ogre::Root*, Ogre::RenderWindow*,
			InGameSettings* settings);
		~GameController();


		ErrorCode LoadMap(); 

		ErrorCode UnloadMap();

		ErrorCode BeginGame();

		ErrorCode EndGame();

		ErrorCode Update(float ms);

		// pause game
		// resume game

		// show menu
		// hide menu
		

	protected:
		void LoadObjectsDefinitions(const string& definitionsDir, const string& modName);
		void LoadTriggers(RefPtr<XmlDocument>& scrFile);
		void LoadGameMod(RefPtr<XmlDocument>& modFile);
		void LoadFactions(const string& factionsDir);
	};
}