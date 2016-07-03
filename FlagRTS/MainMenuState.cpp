#include "MainMenuState.h"
#include <Ogre\Ogre.h>
#include <MainMenu.h>
#include <Managers.h>
#include <Memory.>
#include "AppManager.h"
#include <OISKeyboard.h>
#include <NewGameSettings.h>
#include <InGameSettings.h>
#include "GameLoadingState.h"

namespace FlagRTS
{
	MainMenuState::MainMenuState() :
		_ogre(RenderSystemManager::Instance()->GetOgreRoot()), 
		_mainWindow(RenderSystemManager::Instance()->GetMainWindow()), 
		_loadingStatus(""),
		_menuGui(0),
		_onNewGameRequested(this)
	{

	}

	MainMenuState::~MainMenuState()
	{
		xDeleteSafe(_menuGui);
		Media::AudioManager::Instance()->DestroySoundSource(_soundButtonClickCancel);
		Media::AudioManager::Instance()->DestroySoundSource(_soundButtonClickOk);
	}

	ErrorCode MainMenuState::Load()
	{
		AppManager::Instance()->SheduleTask(xNew1(TaskBuildMenu,this));

		_playerButtonClickOk = Media::AudioManager::Instance()->GetSoundPlayer(
			"ButtonClickOk", "Gui");
		_soundButtonClickOk = _playerButtonClickOk->RequestNewSource();
		_playerButtonClickCancel = Media::AudioManager::Instance()->GetSoundPlayer(
			"ButtonClickCancel", "Gui");
		_soundButtonClickCancel = _playerButtonClickCancel->RequestNewSource();

		return ErrorCodes::Success;
	}

	void MainMenuState::BuildMainMenu()
	{
		_menuGui = xNew0(MainMenu);
		_menuGui->GetNewGameRequestedEvent() += &_onNewGameRequested;
		AppManager::Instance()->NextState();
	}

	ErrorCode MainMenuState::Unload()
	{
		if(_menuGui)
			_menuGui->Unload();
		return ErrorCodes::Success;
	}

	ErrorCode MainMenuState::Start()
	{
		// Set MainMenu scene manager and viewport

		_mainWindow->removeAllViewports();

		_sceneManager = _ogre->createSceneManager(Ogre::ST_GENERIC, "MainMenuSceneManager");
		_camera = _sceneManager->createCamera("MainMenuCamera");
		_camera->setNearClipDistance(1);
		Ogre::Viewport* vp = _mainWindow->addViewport(_camera);
		vp->setBackgroundColour(Ogre::ColourValue(0.1f,0.1f,0.1f));
		_camera->setAspectRatio(vp->getWidth() / vp->getHeight());

		GUIManager::Instance()->ChangeSceneManager(_sceneManager);

		_menuGui->Show();

		CursorManager::Instance()->ChangeCursorContext("Cursor_Normal");
		CursorManager::Instance()->SetCurrentCursorState("Default");
		CursorManager::Instance()->SetVisible(true);

		Media::AudioManager::Instance()->AddMusicToQueue(
			"Music_0", "Gui");

		return ErrorCodes::Success;
	}

	ErrorCode MainMenuState::End()
	{
		_menuGui->Unload();
		Media::AudioManager::Instance()->ClearMusicQueue();

		return ErrorCodes::Success;
	}

	ErrorCode MainMenuState::Update(float ms)
	{
		// update input and gui
		InputManager::Instance()->Update(ms);
		// If esc is pressed - end app
		if(InputManager::Instance()->CheckKeyIsDown(OIS::KC_ESCAPE))
		{
			return ErrorCodes::Exit;
		}

		Media::AudioManager::Instance()->Update(ms);
		GUIManager::Instance()->Update(ms);

		if(InputManager::Instance()->CheckKeyIsDown(OIS::KC_A))
		{
			_playerButtonClickOk->Play(_soundButtonClickOk);
		}

		ErrorCode ec = _menuGui->Update(ms);

		return ec;
	}

	void MainMenuState::NewGameRequested(NewGameSettings* menuSettings)
	{
		// Convert MainMenu settings to GameCore settings 
		// and pass it to GameLoadingState
		InGameSettings* gameSettings = 0; 
		gameSettings = xNew0(InGameSettings);
		XmlNode* mapNode = menuSettings->MapFile->first_node("Map");
		string mapDataFile = menuSettings->MapFileDir + 
			XmlUtility::XmlGetString(mapNode->first_node("MapFile"), "value");
		gameSettings->MapFile = XmlUtility::XmlDocFromFile(mapDataFile.c_str());

		gameSettings->GameModeName = menuSettings->ModeName;
		XmlNode* modeListNode = mapNode->first_node("Modes");
		XmlNode* modeNode = modeListNode->first_node(menuSettings->ModeName.c_str());

		XmlNode* modsListNode = modeNode->first_node("Mods");
		XmlNode* modNode = modsListNode->first_node();
		while(modNode != 0)
		{

			gameSettings->ModsUsed.push_back(XmlUtility::XmlGetString(modNode, "value"));
			modNode = modNode->next_sibling();
		}

		// TODO add default script
		string scriptFile = menuSettings->MapFileDir + 
			XmlUtility::XmlGetStringIfExists(modeNode->first_node("ScriptFile"), "value", "TestMap.mtf");
		gameSettings->ScriptFile = XmlUtility::XmlDocFromFile(scriptFile.c_str());

		RefPtr<Configuration> mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
		string filepath = mainConfig->FindOptionValue("NewGameConfig", "Files");
		RefPtr<XmlDocument> settingsDoc = XmlUtility::XmlDocFromFile(filepath.c_str());
		XmlNode* settingsNode = settingsDoc->first_node("NewGameSettings");

		// Find ResourceModes and convert resource settings
		XmlNode* resourceModesListNode = settingsNode->first_node("ResourceModes");
		XmlNode* resourceModeNode = resourceModesListNode->first_node();
		for(unsigned int i = 0; resourceModeNode != 0; ++i)
		{
			if(i == menuSettings->ResourceMode)
				break;
			resourceModeNode = resourceModeNode->next_sibling();
		}

		// In 'resourceModeNode' should be correct mode
		XmlNode* resourceNode = resourceModeNode->first_node();
		while(resourceNode != 0)
		{
			ResourceSetting resource;
			resource.ResourceName = XmlUtility::XmlGetString(resourceNode, "name");
			resource.StartAmount = XmlUtility::XmlGetInt(resourceNode, "value");
			resource.StartMaxAmount = XmlUtility::XmlGetInt(resourceNode, "max");
			resource.AbsoluteMaxAmount = XmlUtility::XmlGetInt(resourceNode, "abs_max");
			gameSettings->ResourceSettings.push_back(resource);

			resourceNode = resourceNode->next_sibling();
		}

		// Find visibility mode
		XmlNode* visibilityModesListNode = settingsNode->first_node("VisibilityModes");
		XmlNode* visibilityModeNode = visibilityModesListNode->first_node();
		for(unsigned int i = 0; visibilityModeNode != 0; ++i)
		{
			if(i == menuSettings->VisibilityMode)
				break;
			visibilityModeNode = visibilityModeNode->next_sibling();
		}

		gameSettings->FogOfWarEnabled = XmlUtility::XmlGetFlag(visibilityModeNode, "FogOfWar", false);
		gameSettings->ShroudEnabled = XmlUtility::XmlGetFlag(visibilityModeNode, "Shroud", false);

		// Find start loc mode
		XmlNode* startLocModesListNode = settingsNode->first_node("VisibilityModes");
		XmlNode* startLocModeNode = startLocModesListNode->first_node();
		for(unsigned int i = 0; startLocModeNode != 0; ++i)
		{
			if(i == menuSettings->StartLocMode)
				break;
			startLocModeNode = startLocModeNode->next_sibling();
		}

		gameSettings->StartLocationsRandom = XmlUtility::XmlGetFlag(startLocModeNode, "IsRandom", false);

		// At last convert players
		gameSettings->Players.resize(8);
		for(unsigned int i = 0; i < 8; ++i)
		{
			Player* player = 0;
			player = xNew0(Player);
			player->SetIsUsed(false);
			player->SetNumber(i);
			gameSettings->Players[i] = player;
		}

		for(unsigned int pl = 0; pl < menuSettings->Players.size(); ++pl)
		{
			NewGameSettings::PlayerInfo& playerInfo = menuSettings->Players[pl];
			Player* player = gameSettings->Players[playerInfo.Number];
			player->SetNumber( playerInfo.Number );
			player->SetColor( playerInfo.Color );
			player->SetFaction( playerInfo.Faction );
			player->SetIsCpu( playerInfo.IsAI );
			player->SetIsUsed(true);
			player->SetStartLocation( playerInfo.StartLocation );

			// Set relations -> ally with same team
			for(unsigned int pl2 = 0; pl2 < menuSettings->Players.size(); ++pl2)
			{
				if( menuSettings->Players[pl2].Team == playerInfo.Team )
					player->SetRelationWith(menuSettings->Players[pl2].Number, PlayerRelations::Ally);
				else
					player->SetRelationWith(menuSettings->Players[pl2].Number, PlayerRelations::Enemy);
			}

			if(playerInfo.IsHuman)
			{
				gameSettings->ClientPlayer = pl;
			}
		}

		GameLoadingState* gameState = (GameLoadingState*)(AppManager::Instance()->GetState("GameLoading"));
		if(gameState != 0)
		{
			// Set game settings
			gameState->SetGameSettings(gameSettings);
			AppManager::Instance()->PushState("GameLoading");
			AppManager::Instance()->NextState();
		}
	}
}