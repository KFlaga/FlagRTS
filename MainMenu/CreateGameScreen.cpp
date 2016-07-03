#include "CreateGameScreen.h"
#include <GUIManager.h>
#include <LogManager.h>
#include <LocalizationManager.h>
#include <ErrorCodes.h>
#include "MapChooser.h"
#include <ConfigurationManager.h>
#include "MapPreview.h"
#include "PlayerPanel.h"
#include "PlayersSettings.h"
#include "NewGameSettings.h"

namespace FlagRTS
{
	Array<Vector3> CreateGameScreen::PlayersColorsList = Array<Vector3>();
	int CreateGameScreen::NoColorIndex = 0;
	Array<string> CreateGameScreen::ResourceModeNames = Array<string>();
	Array<string> CreateGameScreen::VisibilityModeNames = Array<string>();
	Array<string> CreateGameScreen::StartLocModeNames = Array<string>();
	Array<string> CreateGameScreen::GameTypeNames = Array<string>();
	bool CreateGameScreen::_newGameSettingsLoaded = false;

	CreateGameScreen::CreateGameScreen() :
		IMenuScreen(xNew1(GUI,"CreateGameScreen"), "CreateGameScreen"),
		_loaded(false),
		_onMapSelected(this),
		_onStartPosLeftClicked(this),
		_onStartPosRightClicked(this),
		_selectedStartLoc(-1),
		_onAllReadyChanged(this)
	{
		ADD_LOG("Begin Creating CreateGameScreen");
		LoadNewGameSettings();
		InitGui();
		_loaded = true;
		_buttonStart->setEnabled(false);
		ADD_LOG("End Creating CreateGameScreen");
	}

	CreateGameScreen::~CreateGameScreen()
	{
		Unload();
	}

	ErrorCode CreateGameScreen::Update(float ms)
	{
		_mapList->Update(ms);
		_mapPreview->Update(ms);
		return ErrorCodes::Success;
	}

	void CreateGameScreen::InitGui()
	{
		// InitGui
		if( _gui->LoadLayout("SkirmishSetup.layout", 0, true) != ErrorCodes::Success)
		{
			ADD_LOG("Couldn't load layout : SkirmishSetup.layout");
		}

		AssignWidgets();
		SetWidgetTexts();
		SetEventHandlers();
		FillModesCombos();

		GUIManager::Instance()->RegisterGUI("CreateGameScreen", _gui); 
	}

	void CreateGameScreen::Show()
	{
		GUIManager::Instance()->SetCurrentGUI("CreateGameScreen");
		ResetMap();
		_mapList->Show();
		_mapPreview->Show();
		RefPtr<Configuration> mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
		_mapList->ChangeCurrentDirectory(mainConfig->FindOptionValue("Maps", "Directories"));
		_playersSettings->Show();
	}

	void CreateGameScreen::Hide()
	{
		_playersSettings->Hide();
		_mapPreview->Hide();
		_mapList->Hide();
		IMenuScreen::Hide();
	}

	void CreateGameScreen::Start(MyGUI::Widget* w)
	{
		for(unsigned int i = 0; i < GameTypeNames.size(); ++i)
		{
			if(GameTypeNames[i] == "SingleSkirmish")
			{
				_gameSettings.GameType = i;
				break;
			}
		}
		_gameSettings.MapFile = _currentMapDoc;
		_gameSettings.MapFileDir = _currentMapDir;
		_gameSettings.ModeName = _availableModes[_currentMode].Name;
		_gameSettings.ResourceMode = _currentResourceMode;
		_gameSettings.StartLocMode = _currentStartLocMode;
		_gameSettings.VisibilityMode = _currentVisibilityMode;

		auto& players = _playersSettings->GetPlayerPanels();
		for(unsigned int i = 0; i < players.size(); ++i)
		{
			if(players[i]->GetIsAvailable() == true && 
				players[i]->GetSlot() != PlayerPanel::Closed )
			{
				NewGameSettings::PlayerInfo playerInfo;
				playerInfo.Number = i;
				playerInfo.Faction = players[i]->GetFaction();
				playerInfo.StartLocation = _startLocPositions[players[i]->GetStartPosition()];
				playerInfo.Team = players[i]->GetTeam();
				playerInfo.Color = PlayersColorsList[players[i]->GetColor()];
				playerInfo.IsAI = players[i]->GetSlot() == PlayerPanel::AI;
				playerInfo.IsHuman = players[i]->GetSlot() == PlayerPanel::Human;
				_gameSettings.Players.push_back(playerInfo);
			}
		}

		_gameStartRequested.Fire(&_gameSettings);
	}

	void CreateGameScreen::Return(MyGUI::Widget* w)
	{
		_changeScreenRequested.Fire("MainScreen");
	}

	void CreateGameScreen::Unload()
	{
		if(_gui.IsValid() && _loaded)
		{
			ADD_LOG("Unloading CreateGameScreen");
			GUIManager::Instance()->UnregisterGUI("CreateGameScreen");
			xDeleteSafe(_mapList);
			xDeleteSafe(_mapPreview);
			xDeleteSafe(_playersSettings);
			_gui->UnloadAllLayouts();
			_loaded = false;
		}
	}

	void CreateGameScreen::Resize(int width, int height)
	{

	}

	void CreateGameScreen::AssignWidgets()
	{
		RefPtr<Layout> mainLayout = _gui->GetLayout("SkirmishSetup.layout");
		_bgPanel = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(mainLayout, "MainPanel"));
		_buttonStart = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonStart"));
		_buttonReturn = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonReturn"));
		_labelMaps = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelMaps"));
		_labelPreview = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelPreview"));
		MyGUI::ImageBox* imageMapPreview = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(mainLayout, "ImageMapPreview"));
		_mapPreview = xNew1(MapPreview, imageMapPreview);

		RefPtr<Configuration> mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");

		_mapList = xNew2(MapChooser,
			static_cast<MyGUI::ListBox*>(_gui->FindInLayout(mainLayout, "ListMaps")),
			mainConfig->FindOptionValue("Maps", "Directories"));

		_labelPlayers = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelPlayers"));
		_playersSettings = xNew3(PlayersSettings, this, _gui, mainLayout);

		_labelSettings = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelSettings"));
		_comboGameMode = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboGameMode"));
		_labelGameMode = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelGameMode"));
		_comboStartLocMode = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboStartingLocationsMode"));
		_labelStartLocMode = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelStartLocationsMode"));
		_comboVisibilityMode = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboVisibilityMode"));
		_labelVisibilityMode = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelVisibilityMode"));
		_comboResourcesMode = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboResourcesMode"));
		_labelResourcesMode = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelResourcesMode"));

		_labelUsedMods = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelUsedMods"));
		_listUsedMods = static_cast<MyGUI::ListBox*>(_gui->FindInLayout(mainLayout, "ListUsedMods"));

	}

	void CreateGameScreen::SetWidgetTexts()
	{
		_buttonStart->setCaption(GET_TEXT("ButtonStart","Gui.MainMenu.CreateGameScreen"));
		_buttonReturn->setCaption(GET_TEXT("ButtonReturn","Gui.MainMenu.CreateGameScreen"));
		_labelMaps->setCaption(GET_TEXT("LabelMapList","Gui.MainMenu.CreateGameScreen"));
		_labelPreview->setCaption(GET_TEXT("LabelMapPreview","Gui.MainMenu.CreateGameScreen"));
		_labelPlayers->setCaption(GET_TEXT("LabelPlayers","Gui.MainMenu.CreateGameScreen"));
		_labelSettings->setCaption(GET_TEXT("LabelSettings","Gui.MainMenu.CreateGameScreen"));
		_labelGameMode->setCaption(GET_TEXT("LabelGameMode","Gui.MainMenu.CreateGameScreen"));
		_labelStartLocMode->setCaption(GET_TEXT("LabelStartLocMode","Gui.MainMenu.CreateGameScreen"));
		_labelVisibilityMode->setCaption(GET_TEXT("LabelVisibilityMode","Gui.MainMenu.CreateGameScreen"));
		_labelResourcesMode->setCaption(GET_TEXT("LabelResourcesMode","Gui.MainMenu.CreateGameScreen"));

	}

	void CreateGameScreen::SetEventHandlers()
	{
		_buttonStart->eventMouseButtonClick += MyGUI::newDelegate(this, &CreateGameScreen::Start);
		_buttonReturn->eventMouseButtonClick += MyGUI::newDelegate(this, &CreateGameScreen::Return);

		_mapList->MapSelected() += &_onMapSelected;
		_mapPreview->GetStartPositionLeftClickedEvent() += &_onStartPosLeftClicked;
		_mapPreview->GetStartPositionRightClickedEvent() += &_onStartPosRightClicked;
		_playersSettings->GetAllReadyChangedEvent() += &_onAllReadyChanged;

		_comboGameMode->eventComboChangePosition += MyGUI::newDelegate(this, &CreateGameScreen::ModeSelected);
		_comboResourcesMode->eventComboChangePosition += MyGUI::newDelegate(this, &CreateGameScreen::ResourceModeSelected);
		_comboVisibilityMode->eventComboChangePosition += MyGUI::newDelegate(this, &CreateGameScreen::VisibilityModeSelected);
		_comboStartLocMode->eventComboChangePosition += MyGUI::newDelegate(this, &CreateGameScreen::StartLocModeSelected);
	}

	void CreateGameScreen::MapSelected(RefPtr<XmlDocument>& mapDoc)
	{
		_currentMapDoc = mapDoc;
		_currentMapDir = _mapList->GetCurrentDir();
		_buttonStart->setEnabled(false);
		try
		{
			// Read and set preview image and start locations
			XmlNode* mapNode = mapDoc->first_node("Map");
			Vector2 mapSize = XmlUtility::XmlGetXY(mapNode->first_node("Size"));
			string previewImage = XmlUtility::XmlGetString(mapNode->first_node("PreviewImage"), "value");
			XmlNode* startLocListNode = mapNode->first_node("AvailableStartLocations");
			XmlNode* startLocNode = startLocListNode->first_node();
			while( startLocNode != 0 )
			{
				_startLocPositions.push_back(XmlUtility::XmlGetXY(startLocNode));
				startLocNode = startLocNode->next_sibling();
			}

			_mapPreview->SetNewMap(mapSize, _mapList->GetCurrentDir() + previewImage, 
				previewImage, _startLocPositions.size());
			for(unsigned int i = 0; i < _startLocPositions.size(); ++i)
			{
				_mapPreview->MoveStartingPosition(i, _startLocPositions[i]);
				_mapPreview->SetStartingPositionColor(i, PlayersColorsList[NoColorIndex]);
			}

			AddGameModes(mapNode);
			int defMode = _currentMode;
			_currentMode = -1;
			SetGameMode(defMode);

			_comboGameMode->setEnabled(true);
			_comboResourcesMode->setEnabled(true);
			_comboStartLocMode->setEnabled(true);
			_comboVisibilityMode->setEnabled(true);
		}
		catch(...)
		{
			_ASSERT(false);
			// Map file is corrupted, maybe show some info on it
		}
	}

	void CreateGameScreen::StartPosLeftClicked(int num)
	{
		// TODO add StartLocation class, which will handle selection/deselection/player_ready etc logic inside

		// If not assigned location is clicked :
		//	- If start loc is selected, then switch players, deselect
		//	- If not, then assign next unassgined player
		//	- If there's no such player, do nothing
		// If assigned location is selected :
		//	- If start loc is selected, then switch players, deselect
		//	- If not, then select this loc
		// If loc clicked is selected, then deselect
		if( _selectedStartLoc != -1 )
		{
			if( num != _selectedStartLoc )
			{
				// Switch players -> works also if clicked loc is empty
				_playersSettings->SwitchStartLocationAssignments(num, _selectedStartLoc);
			}
			// Deselect
			_mapPreview->DisableStartPositionHighlight(_selectedStartLoc);
			_selectedStartLoc = -1;
		}
		else if( _playersSettings->GetStartLocationsPlayer(num) == -1 )
		{
			// Not assigned loc selected and no other is selected -> assign next unassgined
			_playersSettings->AssignNextPlayerToStartLocation(num);
		}
		else
		{
			// Assigned loc is selected and no other is selected -> select this one
			_selectedStartLoc = num;
			_mapPreview->EnableStartPositionHighlight(_selectedStartLoc);
		}
	}

	void CreateGameScreen::StartPosRightClicked(int num)
	{
		// Unoccupy start loc if is occcupied, deselect start loc
		_playersSettings->DeassignStartLocation(num);

		if( _selectedStartLoc != -1 )
		{
			_mapPreview->DisableStartPositionHighlight(_selectedStartLoc);
			_selectedStartLoc = -1;
		}
	}

	void CreateGameScreen::AllPlayersReadyChanged(bool ready)
	{
		_buttonStart->setEnabled(ready);
	}

	void CreateGameScreen::AddGameModes(XmlNode* mapNode)
	{
		string defMode = XmlUtility::XmlGetString(mapNode->first_node("DefaultMode"), "name");
		_currentMode = 0;
		_availableModes.clear();
		_comboGameMode->removeAllItems();

		XmlNode* modesListNode = mapNode->first_node("Modes");
		int modeNum = 0;
		XmlNode* modeNode = modesListNode->first_node();
		while(modeNode != 0)
		{
			Mode mode;
			mode.Name = modeNode->name();
			mode.Display = XmlUtility::XmlGetStringIfExists(modeNode, "display", mode.Name.c_str());
			mode.IsCustomisable = XmlUtility::XmlGetBoolIfExists(modeNode, "customisable", true);
			_availableModes.push_back(mode);
			_comboGameMode->addItem(mode.Display, modeNum);

			// Additionaly find default mode index
			if(mode.Name.compare(defMode) == 0)
			{
				_currentMode = modeNum;
				_comboGameMode->setIndexSelected(modeNum);
			}

			++modeNum;
			modeNode = modeNode->next_sibling();
		}
	}

	void CreateGameScreen::ModeSelected(MyGUI::ComboBox* sender, size_t index)
	{
		int newMode = *_comboGameMode->getItemDataAt<int>(index, false);
		SetGameMode(newMode);
	}

	void CreateGameScreen::SetGameMode(int mode)
	{
		if(mode != _currentMode)
		{
			_currentMode = mode;

			// Reset selection
			if(_selectedStartLoc != -1)
			{
				_mapPreview->DisableStartPositionHighlight(_selectedStartLoc);
				_selectedStartLoc = -1;
			}

			// Colors / players should be reseted here
			_playersSettings->SetMode(_currentMapDoc->first_node("Map"), 
				_availableModes[_currentMode].Name);
		}
	}

	void CreateGameScreen::FillModesCombos()
	{
		for(int i = 0; i < ResourceModeNames.size(); ++i)
		{
			_comboResourcesMode->addItem(ResourceModeNames[i], i);
		}
		_comboResourcesMode->setIndexSelected(0);
		_currentResourceMode = 0;

		for(int i = 0; i < VisibilityModeNames.size(); ++i)
		{
			_comboVisibilityMode->addItem(VisibilityModeNames[i], i);
		}
		_comboVisibilityMode->setIndexSelected(0);
		_currentVisibilityMode = 0;

		for(int i = 0; i < StartLocModeNames.size(); ++i)
		{
			_comboStartLocMode->addItem(StartLocModeNames[i], i);
		}
		_comboStartLocMode->setIndexSelected(0);
		_currentStartLocMode = 0;
	}

	void CreateGameScreen::ResourceModeSelected(MyGUI::ComboBox* sender, size_t index)
	{
		_currentResourceMode = index;
	}

	void CreateGameScreen::VisibilityModeSelected(MyGUI::ComboBox* sender, size_t index)
	{
		_currentVisibilityMode = index;
	}

	void CreateGameScreen::StartLocModeSelected(MyGUI::ComboBox* sender, size_t index)
	{
		_currentStartLocMode = index;
	}

	void CreateGameScreen::ResetMap()
	{
		_currentMapDoc = 0;
		_currentMode = -1;
		_selectedStartLoc = -1;
		_availableModes.clear();
		_comboGameMode->removeAllItems();

		_comboGameMode->setEnabled(false);
		_comboGameMode->setCaption("");
		_comboResourcesMode->setEnabled(false);
		_comboStartLocMode->setEnabled(false);
		_comboVisibilityMode->setEnabled(false);
		_mapPreview->ResetMap();
		_playersSettings->ResetMode();
	}

	void CreateGameScreen::LoadNewGameSettings()
	{
		if(_newGameSettingsLoaded == false)
		{
			_newGameSettingsLoaded = true;

			// Get new game settings config file
			RefPtr<Configuration> mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
			string filepath = mainConfig->FindOptionValue("NewGameConfig", "Files");
			RefPtr<XmlDocument> settingsDoc = XmlUtility::XmlDocFromFile(filepath.c_str());
			XmlNode* rootNode = settingsDoc->first_node("NewGameSettings");
			// Read values
			XmlNode* gameTypesListNode = rootNode->first_node("GameTypes");
			XmlNode* gameTypeNode = gameTypesListNode->first_node();
			while(gameTypeNode != 0)
			{
				GameTypeNames.push_back(XmlUtility::XmlGetString(gameTypeNode, "name"));
				gameTypeNode = gameTypeNode->next_sibling();
			}

			XmlNode* colorsListNode = rootNode->first_node("AvailableColors");
			XmlNode* colorNode = colorsListNode->first_node();
			while(colorNode != 0)
			{
				PlayersColorsList.push_back(XmlUtility::XmlGetRGB(colorNode));
				colorNode = colorNode->next_sibling();
			}
			NoColorIndex = PlayersColorsList.size() - 1;

			XmlNode* resourceModesListNode = rootNode->first_node("ResourceModes");
			XmlNode* resourceModeNode = resourceModesListNode->first_node();
			while(resourceModeNode != 0)
			{
				ResourceModeNames.push_back(XmlUtility::XmlGetString(resourceModeNode, "name"));
				resourceModeNode = resourceModeNode->next_sibling();
			}

			XmlNode* visibilityModesListNode = rootNode->first_node("VisibilityModes");
			XmlNode* visibilityModeNode = visibilityModesListNode->first_node();
			while(visibilityModeNode != 0)
			{
				VisibilityModeNames.push_back(XmlUtility::XmlGetString(visibilityModeNode, "name"));
				visibilityModeNode = visibilityModeNode->next_sibling();
			}

			XmlNode* startModesListNode = rootNode->first_node("StartLocModes");
			XmlNode* startModeNode = startModesListNode->first_node();
			while(startModeNode != 0)
			{
				StartLocModeNames.push_back(XmlUtility::XmlGetString(startModeNode, "name"));
				startModeNode = startModeNode->next_sibling();
			}
		}
	}
}