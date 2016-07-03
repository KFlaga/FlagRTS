#pragma once

#include <Event.h>
#include <Ogre\OgreVector4.h>
#include <Array.h>
#include <NoticeMessages.h>

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class MainPanel;
	}

	class CommandButton;
	class UnitGroup;
	class Unit;
	struct TechBuilding;

	struct GuiArea
	{
		int Left, Top, Width, Height;

		int GetLeft() const { return Left; }
		int GetTop() const { return Top; }
		int Get() const { return Width; }
		int GetHeight() const { return Height; }
		int GetRight() const { return Left + Width; }
		int GetBottom() const { return Top + Height; }

		void ParseFromConfig(const string& areaValue);
		bool IsHit(int x, int y) const;
		bool IsHit(const Vector2& pos) const;
	};

	// Creates and shows/hides in-game gui
	// Sets all interaction between gui and other parts of game
	class GuiController
	{
	private:
		GameGui::MainPanel* _mainPanel;

		Array<string> _guiFiles;
		Array<GuiArea> _guiAreas;
		string _prevCursor;
		bool _isCursorOverGui;
		int _selectedUnitsCount;

	public:
		GuiController(Ogre::SceneManager* ogreMgr);
		~GuiController();

		// Parses gui files from dir ( files to look for specified in gui config)
		void AddModGuiDir(const string& dir);

		// Creates gui ( should be called after game is fully loaded )
		void CreateGui();

		void Update(float ms);

		GameGui::MainPanel* GetMainPanel() { return _mainPanel; }

		void ShowGui();
		void HideGui();
		bool IsCursorOverGui() const { return _isCursorOverGui; }

		void UpdateSelectedUnitGroup(UnitGroup* selectedUnits);
		void UpdateSelectedUnitContextSubgroup(Array<Unit*>* contextSubgroup);
		Event<Unit*, bool>& UnitButtonClicked();

		void RegisterCommandButton(CommandButton* button);
		void ResetCommandButtons();
		Event<CommandButton*>& CommandChoosen();

		void AvailableBuildingsChanged(const Array<TechBuilding*>& newBuildings);
		Event<TechBuilding*>& BuildingButtonClicked();

		// Shows a message above BottomPanel ( quick fade, best for info on events that need short, fast notice,
		// like 'insufficient resources', 'invalid location', 'inaccessible action' etc )
		void ShowQuickNotice(const NoticeMessage& msg);

		// Shows notice on left side of screen. Fades after couple of seconds, few messages may
		// be shown at one time
		void ShowNotice(const NoticeMessage& msg);

		void ShowConsole();
		void HideConsole();
		bool IsConsoleVisible();
		void PrintConsoleOutput(const string& output);
		Event<Array<string>&>& ConsoleQueryAccepted();

		// Event occurs when mouse is moved over minimap with left button pressed
		// Parameter is mouse position in minimap coords
		Event<Vector2>& MinimapLeftButtonDragged();
		// Event occurs when left button is clicked over minimap ( pressed/released in same position )
		// Parameter is mouse position in minimap coords
		Event<Vector2>& MinimapLeftButtonClicked();
		// Event occurs when mouse is moved over minimap with left button pressed
		// Parameter is mouse position in minimap coords
		Event<Vector2>& MinimapRightButtonDragged();
		// Event occurs when left button is clicked over minimap ( pressed/released in same position )
		// Parameter is mouse position in minimap coords
		Event<Vector2>& MinimapRightButtonClicked();

	private:
		void ParseGuiAreas();
		bool CheckIsCursorOverGui(const Vector2& pos);

		void ResizeGui(std::pair<int,int> size);
		typedef std::pair<int,int> SizePair;
		DEFINE_DELEGATE1(ResizeGuiDelegate, GuiController, 
			ResizeGui, SizePair);
		ResizeGuiDelegate _resizeGui;

		void SwitchFunctionalPanels(int newPanel);
		DEFINE_DELEGATE1(FunctionalPanelSwitchDelegate, GuiController, 
			SwitchFunctionalPanels, int);
		FunctionalPanelSwitchDelegate _onFuncPanelSwitch;

		DEFINE_DELEGATE1(ShowNoticeDelegate, GuiController, 
			ShowNotice, const NoticeMessage&);
		ShowNoticeDelegate _showNotice;

		DEFINE_DELEGATE1(ShowQuickNoticeDelegate, GuiController, 
			ShowQuickNotice, const NoticeMessage&);
		ShowQuickNoticeDelegate _showQuickNotice;
	};
}