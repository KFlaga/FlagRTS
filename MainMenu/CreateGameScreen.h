#include "IMenuScreen.h"
#include <Event.h>
#include <Xml.h>
#include "NewGameSettings.h"

namespace FlagRTS
{
	class MapChooser;
	class CreateGameScreen;
	class MapPreview;
	class PlayerPanel;
	class PlayersSettings;

	struct Mode
	{
		string Name;
		string Display;
		bool IsCustomisable;
	};

	// Main panel screen with buttons for changing to other screen
	class CreateGameScreen : public IMenuScreen
	{
	public:
		static Array<Vector3> PlayersColorsList;
		static int NoColorIndex;
		static Array<string> ResourceModeNames;
		static Array<string> VisibilityModeNames;
		static Array<string> StartLocModeNames;
		static Array<string> GameTypeNames;

	private:
		static bool _newGameSettingsLoaded;

	private:
		NewGameSettings _gameSettings;
		MyGUI::ImageBox* _bgPanel;
		MyGUI::Button* _buttonReturn;
		MyGUI::Button* _buttonStart;

		MyGUI::EditBox* _labelPreview;
		MapPreview* _mapPreview;

		MyGUI::EditBox* _labelMaps;
		MapChooser* _mapList;

		MyGUI::EditBox* _labelPlayers;
		PlayersSettings* _playersSettings;
		
		MyGUI::EditBox* _labelSettings;
		MyGUI::Widget* _panelSettings;
		MyGUI::EditBox* _labelGameMode;
		MyGUI::ComboBox* _comboGameMode;
		MyGUI::EditBox* _labelStartLocMode;
		MyGUI::ComboBox* _comboStartLocMode;
		MyGUI::EditBox* _labelVisibilityMode;
		MyGUI::ComboBox* _comboVisibilityMode;
		MyGUI::EditBox* _labelResourcesMode;
		MyGUI::ComboBox* _comboResourcesMode;

		MyGUI::EditBox* _labelUsedMods;
		MyGUI::ListBox* _listUsedMods;
		
		RefPtr<XmlDocument> _currentMapDoc;
		string _currentMapDir;
		Array<Mode> _availableModes;
		Array<Vector2> _startLocPositions;
		int _currentMode;
		int _currentResourceMode;
		int _currentVisibilityMode;
		int _currentStartLocMode;

		int _selectedStartLoc; // Selected start location or -1
		
		bool _loaded; // True if gui is loaded

		Event<NewGameSettings*> _gameStartRequested;
	
	public:
		CreateGameScreen();
		~CreateGameScreen();

		ErrorCode Update(float ms);

		void Show();
		void Unload();
		void Hide();

		void Resize(int width, int height);

		MapPreview* GetMapPreview() { return _mapPreview; }

		Event<NewGameSettings*>& GetNewGameStartRequestedEvent() { return _gameStartRequested; }

	private:
		void LoadNewGameSettings();
		void InitGui();
		void Start(MyGUI::Widget*);
		void Return(MyGUI::Widget*);

		void AssignWidgets();
		void SetWidgetTexts();
		void SetEventHandlers();
		void FillModesCombos();
		void ResetMap();

		void AddGameModes(XmlNode* mapNode);
		void SetGameMode(int mode);
		void ModeSelected(MyGUI::ComboBox* sender, size_t index);
		void ResourceModeSelected(MyGUI::ComboBox* sender, size_t index);
		void VisibilityModeSelected(MyGUI::ComboBox* sender, size_t index);
		void StartLocModeSelected(MyGUI::ComboBox* sender, size_t index);

		void MapSelected(RefPtr<XmlDocument>& mapDoc);
		DEFINE_DELEGATE1(MapSelectedDelegate, CreateGameScreen,
			MapSelected, RefPtr<XmlDocument>&);
		MapSelectedDelegate _onMapSelected;
		
		void StartPosLeftClicked(int num);
		DEFINE_DELEGATE1(StartPosLeftClickedDelegate, CreateGameScreen,
			StartPosLeftClicked, int);
		StartPosLeftClickedDelegate _onStartPosLeftClicked;
		
		void StartPosRightClicked(int num);
		DEFINE_DELEGATE1(StartPosRightClickedDelegate, CreateGameScreen,
			StartPosRightClicked, int);
		StartPosRightClickedDelegate _onStartPosRightClicked;

		void AllPlayersReadyChanged(bool ready);
		DEFINE_DELEGATE1(AllPlayersReadyChangedDelegate, CreateGameScreen,
			AllPlayersReadyChanged, bool);
		AllPlayersReadyChangedDelegate _onAllReadyChanged;
	};
}