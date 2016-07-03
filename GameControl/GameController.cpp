#include "GameController.h"

#include <ErrorCodes.h>
#include "CameraController.h"
#include <GameWorld.h>
#include <MainGameObjectPool.h>
#include <MainEventPool.h>
#include "InteractionController.h"
#include "SelectionController.h"
#include "TechnologyController.h"
#include <InputManager.h>
#include <CursorManager.h>
#include <Player.h>
#include "GuiController.h"
#include <RenderSystemManager.h>
#include <SelectionBox.h>
#include "GameGuiMainPanel.h"
#include <MyGUI\MyGUI_Widget.h>
#include <Map.h>
#include <KeyBindings.h>
#include <PathingSystem.h>
#include <DllGameMod.h>
#include <ConstructionManager.h>
#include <PlayerStatsManager.h>
#include <TechnologyManager.h>
#include "Console.h"
#include <AudioManager.h>
#include <CursorManager.h>
#include <fstream>
#include <sstream>
#include <FileBrowser.h>
#include <FactionsManager.h>
#include <Profiler.h>

namespace FlagRTS
{
	class ConsoleBrigde : public IConsolePrinter
	{
	protected:
		GuiController* _guiCtrl;
		Console* _console;

		DEFINE_DELEGATE1(ResolveQueryDelegate, ConsoleBrigde, 
			ResolveQuery, Array<string>&);
		ResolveQueryDelegate _resolveQueryHandler;

	public:
		ConsoleBrigde(GuiController* gui, Console* console) :
			_guiCtrl(gui),
			_console(console),
			_resolveQueryHandler(this)
		{ 
			_guiCtrl->ConsoleQueryAccepted() += &_resolveQueryHandler;
			_console->SetConsolePrinter(this);
		}

		~ConsoleBrigde()
		{
			_guiCtrl->ConsoleQueryAccepted() -= &_resolveQueryHandler;
		}

		void Print(const string& output)
		{
			_guiCtrl->PrintConsoleOutput(output);
		}

		void ResolveQuery(Array<string>& query)
		{
			_console->ResolveQuery(query);
		}
	};

	GameController::GameController(Ogre::Root* ogre, Ogre::RenderWindow* window,
		InGameSettings* settings) :
	_ogreRoot(ogre), 
		_window(window), 
		_gameSettings(*settings),
		_GameWorld(0),
		_camController(0),
		_objectPool(0), 
		_eventPool(0), 
		_inputController(0),
		_selectionController(0),
		_guiController(0),
		_pathingSystem(0),
		_techController(0),
		_statsManager(0),
		_techsManager(0),
		_console(0),
		_consoleBridge(0)
	{
		_ogreSceneMgr = _ogreRoot->createSceneManager(Ogre::ST_GENERIC);
		MovableObject::setDefaultQueryFlags(0);
		_objectPool = xNew0(MainGameObjectPool);
		MainGameObjectPool::GlobalPool = _objectPool;
		_statsManager = xNew1(GlobalStatisticsManager, (uint8)settings->Players.size());
		_GameWorld = xNew4(GameWorld, _ogreSceneMgr, window, _objectPool, _statsManager);
		GameWorld::GlobalWorld = _GameWorld;
		_eventPool = xNew0(MainEventPool);
		MainEventPool::GlobalPool = _eventPool;
	}

	GameController::~GameController()
	{
		for(unsigned int i = 0; i < _loadedMods.size(); ++i)
		{
			DllPlugin::ReleasePlugin(_loadedMods[i]);
		}
		_loadedMods.clear();

		xDeleteSafe(_inputController);
		xDeleteSafe(_consoleBridge);
		xDeleteSafe(_console);
		xDeleteSafe(_guiController);
		xDeleteSafe(_selectionController);
		xDeleteSafe(_camController);
		xDeleteSafe(_techController);
		xDeleteSafe(_eventPool);
		xDeleteSafe(_pathingSystem);
		xDeleteSafe(_GameWorld);
		xDeleteSafe(_statsManager);
		xDeleteSafe(_objectPool);
	}


	ErrorCode GameController::LoadMap()
	{
		PROFILE_REGISTER_CLOCK(loadClock, "GameController::LoadMap");
		PROFILE_START(loadClock);

		_camController = xNew3(CameraController, _ogreSceneMgr, _window, VIEWPORT_GAME);
		_selectionController = xNew2(SelectionController, _camController, _ogreSceneMgr);
		_guiController = xNew1(GuiController, _ogreSceneMgr);
		_console = xNew0(Console);

		// 1) Set settins for GameWorld and load it
		_GameWorld->SetGameSettings(&_gameSettings);
		_GameWorld->LoadTerrain();
		_GameWorld->CreateGameManagers();
		_pathingSystem = _GameWorld->GetPathingSystem();

		// 2) Load base definitions
		auto& mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");	
		string baseDefsDir = mainConfig->FindOption("Resources" , "Directories")->Value + "Definitions\\";
		LoadObjectsDefinitions(baseDefsDir, "Base");

		// 3) Load all mods
		string modsDir = mainConfig->FindOption("Mods" , "Directories")->Value;
		for(unsigned int i = 0; i < _gameSettings.ModsUsed.size(); ++i)
		{
			// Open mod file and load it
			RefPtr<XmlDocument> modDoc = 
				XmlUtility::XmlDocFromFile((modsDir + _gameSettings.ModsUsed[i] + ".mod").c_str());
			LoadGameMod(modDoc);
		}

		// Have all loaded, so can init mods
		for(unsigned int i = 0; i < _loadedMods.size(); ++i)
		{
			_loadedMods[i]->SetGameWorld(_GameWorld);
			_loadedMods[i]->Init();
		}

		// 4) Load spawn infos ( needed all defs loaded )
		_GameWorld->LoadSpawnInfos();

		// 5) Finish loading object definitions
		_GameWorld->ActivateAllRegisteredSuppliers();
		_objectPool->GetDefinitionsCreator()->LoadingFinished();

		// 6) Load custom scripts
		LoadTriggers(_gameSettings.ScriptFile);

		_GameWorld->CreateWorld();
		
		_techsManager = _GameWorld->GetTechnologyManager();
		_techController = xNew2(TechnologyController, _techsManager, _GameWorld->GetPlayers()->GetClientPlayer());

		_guiController->CreateGui();
		_selectionController->GetSelectionBox()->GetBox()->
			attachToWidget(_guiController->GetMainPanel()->GetMainPanel(), MyGUI::WidgetStyle::Child);
		_inputController = xNew5(InteractionController, 
			_ogreSceneMgr, _selectionController, _guiController, _techController, _camController);
		_consoleBridge = xNew2(ConsoleBrigde, _guiController, _console);


		PROFILE_END(loadClock);
		return ErrorCodes::Success;
	}

	void GameController::LoadObjectsDefinitions(const string& definitionsDir, const string& modName)
	{
		PROFILE_REGISTER_CLOCK(loadClock, "GameController::LoadObjectsDefinitions");
		PROFILE_START(loadClock);
		// Load file from acquired path and get as string
		std::ifstream defFile;
		defFile.open(definitionsDir + "Definitions.xml");
		std::stringstream defFileSStream;
		defFileSStream << defFile.rdbuf();
		string defFileContents = defFileSStream.str();
		// Parse string as XmlDoc
		RefPtr<XmlDocument> defDoc = XmlUtility::XmlDocFromString(defFileContents.c_str());
		// Load objects from file ( root node = ObjectDefinitions )
		_objectPool->LoadAllDefinitions(defDoc->first_node("ObjectDefinitions"), modName);

		defFile.close();
		PROFILE_END(loadClock);
	}

	void GameController::LoadTriggers(RefPtr<XmlDocument>& scrFile)
	{
		if( scrFile.IsValid() )
			_eventPool->ParseAllTriggers(scrFile->first_node("MapTriggers"));
	}

	void GameController::LoadGameMod(RefPtr<XmlDocument>& modFile)
	{
		XmlNode* rootNode = modFile->first_node("GameMod");
		string modName = XmlUtility::XmlGetString(rootNode->first_node("Name"), "value");
		auto& mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");

		bool haveResources = XmlUtility::XmlGetBoolIfExists(rootNode->first_node("HaveResources"), "value", false);
		if( haveResources )
		{
			// If mod adds new resources : add resource locations for ogre / managers / load definitions
			string modsDir = mainConfig->FindOption("Mods" , "Directories")->Value;
			string resDir = modsDir + 
				XmlUtility::XmlGetString(rootNode->first_node("ResourcesDir"), "value") + "\\";

			XmlNode* resNode = rootNode->first_node("Resources");
			if( XmlUtility::XmlGetFlag(resNode, "HaveGui") )
				_guiController->AddModGuiDir(resDir + "Gui\\");

			if( XmlUtility::XmlGetFlag(resNode, "HaveSounds") )
				Media::AudioManager::Instance()->AddSoundDoc(resDir + "Sounds\\");

			if( XmlUtility::XmlGetFlag(resNode, "HaveCursors") )
				CursorManager::Instance()->AddCursors(resDir + "Cursors\\");

			if( XmlUtility::XmlGetFlag(resNode, "HaveMesh") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Mesh", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveMaterials") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Materials", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveTextures") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Textures", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveEffects") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Effects", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveFonts") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Fonts", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveDefinitions") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Definitions", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveFactions") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Factions", "FileSystem", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveMaps") )
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				resDir + "Maps", "FileSystem", "Maps");

			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveDefinitions") )
				LoadObjectsDefinitions(resDir + "Definitions\\", modName);

			if( XmlUtility::XmlGetFlag(resNode, "HaveFactions") )
				LoadFactions(resDir + "Factions\\");

		}

		bool haveDll = XmlUtility::XmlGetBoolIfExists(rootNode->first_node("HaveDll"), "value", false);
		if( haveDll )
		{
			// If mod have a dll file, create DllGameMod
			string dllName = XmlUtility::XmlGetString(rootNode->first_node("DllName"), "value");
			// Find dll mods directory in main config and load mod
			string dllPath = mainConfig->FindOption("ModsDll" , "Directories")->Value + dllName;
			DllPluginGlobalInstances globals;
#ifdef _DEBUG
			DllGameMod* mod = (DllGameMod*)DllPlugin::GetPlugin((dllPath+"_d").c_str(), &globals);
#else
			DllGameMod* mod = (DllGameMod*)DllPlugin::GetPlugin(dllPath.c_str(), &globals);
#endif
			if(mod != 0)
				_loadedMods.push_back(mod);
		}
	}

	void GameController::LoadFactions(const string& factionsDir)
	{
		// Parse each .faction file in folder
		FileBrowser fileBrowser;
		fileBrowser.SetDirectory(factionsDir);
		fileBrowser.SetSort(false);
		fileBrowser.UpdateFiles();

		auto& files = fileBrowser.GetFilesWithExtension("faction");
		for(unsigned int i = 0; i < files.size(); ++i)
		{
			_GameWorld->GetFactionsManager()->LoadFaction(factionsDir + files[i]);
		}
	}

	ErrorCode GameController::UnloadMap()
	{
		PROFILE_REGISTER_CLOCK(loadClock, "GameController::UnloadMap");
		PROFILE_START(loadClock);

		_eventPool->RemoveAllTriggers();
		_GameWorld->DeactivateAllRegisteredSuppliers();

		for(unsigned int i = 0; i < _loadedMods.size(); ++i)
		{
			_loadedMods[i]->Deinit();
			DllPlugin::ReleasePlugin(_loadedMods[i]);
		}

		_GameWorld->UnloadMap();
		_objectPool->UnloadAllResources(_ogreSceneMgr);

		PROFILE_END(loadClock);
		return ErrorCodes::Success;
	}

	ErrorCode GameController::BeginGame()
	{
		// Set ingame cursor/gui/sounds etc ( depends on race )
		CursorManager::Instance()->ChangeCursorContext("Cursor_Game_Normal");
		CursorManager::Instance()->SetCurrentCursorState("Default");
		
		// Check all requirements before starting map ( so they are set to default values )
		_techController->CheckAllRequirements();

		// Fire all Initialisation triggers before game starts
		_eventPool->MapInitialised();

		_GameWorld->ShowWorld();

		// Camera should hover over client start position
		_camController->BeginCameraRender();
		_guiController->ShowGui();
		_inputController->BeginInputProcessing();
		return ErrorCodes::Success;
	}

	ErrorCode GameController::EndGame()
	{
		_guiController->HideGui();
		_camController->EndCameraRender();
		_GameWorld->HideWorld();

		return ErrorCodes::Success;
	}

	void ResetProfileClocks();

	ErrorCode GameController::Update(float ms)
	{
		ResetProfileClocks();
		PROFILE_REGISTER_CLOCK(updateClock, "GameController::Update");
		PROFILE_REGISTER_CLOCK(camClock, "GameController::CameraUpdate");
		PROFILE_REGISTER_CLOCK(guiClock, "GameController::GuiUpdate");
		PROFILE_REGISTER_CLOCK(objectsClock, "GameController::ObjectPoolUpdate");
		PROFILE_REGISTER_CLOCK(eventClock, "GameController::EventUpdate");
		PROFILE_REGISTER_CLOCK(inputClock, "GameController::InputUpdate");
		PROFILE_REGISTER_CLOCK(pathClock, "GameController::PathingUpdate");
		PROFILE_REGISTER_CLOCK(cpClock, "GameController::ConstructionUpdate");

		PROFILE_START_NEW(updateClock);
		PROFILE_START_NEW(camClock);
		_camController->Update(ms);
		PROFILE_END(camClock);

		PROFILE_START_NEW(guiClock);
		_guiController->Update(ms);
		PROFILE_END(guiClock);

		PROFILE_START_NEW(objectsClock);
		_objectPool->UpdateAll(ms);
		PROFILE_END(objectsClock);

		PROFILE_START_NEW(eventClock);
		_eventPool->Update(ms);
		PROFILE_END(eventClock);

		PROFILE_START_NEW(inputClock);
		_inputController->Update(_camController->GetCastingRay(
			InputManager::Instance()->GetMouseScreenPos()));
		PROFILE_END(inputClock);

		PROFILE_START_NEW(pathClock);
		_pathingSystem->Update(ms);
		PROFILE_END(pathClock);

		PROFILE_START_NEW(cpClock);
		_GameWorld->GetConstructionManager()->Update(ms);
		PROFILE_END(cpClock);

		_GameWorld->Update(ms);

		PROFILE_END(updateClock);
		return ErrorCodes::Success;
	}

	void ResetProfileClocks()
	{
	}
}