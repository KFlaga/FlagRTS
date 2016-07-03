#include "GuiController.h"
#include <Ogre\OgreSceneManager.h>
#include <RenderSystemManager.h>
#include <InputManager.h>
#include <ConfigurationManager.h>

#include <GUIManager.h>
#include "GameGuiMainPanel.h"
#include "GameGuiCommandButtonsPanel.h"
#include "GameGuiUnitPanel.h"
#include "GameGuiSelectionPanel.h"
#include "GameGuiPerformancePanel.h"
#include "GameGuiBuildingPanel.h"
#include "GameGuiFunctionalButtonsPanel.h"
#include "GameGuiNotifyPanel.h"
#include "GameGuiConsole.h"
#include "GameGuiMinimapPanel.h"
#include <KeyBindings.h>
#include <CursorManager.h>
#include <GameWorld.h>

#include <UnitGroup.h>
#include <Unit.h>
#include <Player.h>

namespace FlagRTS
{
	GuiController::GuiController(Ogre::SceneManager* ogreMgr) :
		_mainPanel(0),
		_resizeGui(this),
		_prevCursor("Default"),
		_onFuncPanelSwitch(this),
		_selectedUnitsCount(0),
		_showNotice(this),
		_showQuickNotice(this)
	{
		GUIManager::Instance()->ChangeSceneManager(ogreMgr);
		GUIManager::Instance()->ChangeViewport(VIEWPORT_GAME);
		ParseGuiAreas();

		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		// Load all resource files and save file names to load, so they will be searched for in mods
		SectionIterator sectionIt, end;
		conf->Begin("Files", sectionIt);
		conf->End("Files", end);
		for(; sectionIt != end; ++sectionIt)
		{
			_guiFiles.push_back(sectionIt->Value->Value);
			MyGUI::ResourceManager::getInstance().load( sectionIt->Value->Value );
		}
	}

	GuiController::~GuiController()
	{
		xDeleteSafe(_mainPanel);
	}

	void GuiController::AddModGuiDir(const string& dir)
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
			dir, "FileSystem", "MyGUI");

		// Open gui files as std streams and create/load mygui xmls
		for(unsigned int i = 0; i != _guiFiles.size(); ++i)
		{
			std::ifstream fileStream;
			fileStream.open(dir + _guiFiles[i]);
			if( fileStream.is_open() )
			{
				MyGUI::xml::Document doc;
				doc.open(fileStream);

				MyGUI::ResourceManager::getInstance().loadFromXmlNode(doc.getRoot(), "", 
					MyGUI::Version(MYGUI_VERSION_MAJOR, MYGUI_VERSION_MINOR, MYGUI_VERSION_PATCH));

				fileStream.close();
			}
		}
	}

	void GuiController::CreateGui()
	{
		_mainPanel = xNew0(GameGui::MainPanel);
		// _mainPanel->GetFunctionalPanel<GameGui::BuildingPanel>(GameGui::Buildings)->ButtonClicked();
		_mainPanel->GetFunctionalButtonsPanel()->FunctionalPanelSwitchRequested() +=
			&_onFuncPanelSwitch;

		int screenWidth = RenderSystemManager::Instance()->GetMainWindow()->getWidth();
		int screenHeight = RenderSystemManager::Instance()->GetMainWindow()->getHeight();
		ResizeGui(std::make_pair(screenWidth, screenHeight));

		RenderSystemManager::Instance()->WindowResized() += &_resizeGui;

		GameWorld::GlobalWorld->NoticeRequested() += &_showNotice;
		GameWorld::GlobalWorld->QuickNoticeRequested() += &_showQuickNotice;
	}

	void GuiController::Update(float ms)
	{
		bool lastTimeOverGui = _isCursorOverGui;

		auto input = InputManager::Instance()->GetInputState();
		_isCursorOverGui = CheckIsCursorOverGui(input->GetMouseScreenPosAbs());

		// If cursor hover changes from map to gui, change cursor state to Gui
		// when it returns set to Default
		if(lastTimeOverGui == true &&
			_isCursorOverGui == false)
		{
			CursorManager::Instance()->SetCurrentCursorState(_prevCursor);
		}
		else if(lastTimeOverGui == false &&
			_isCursorOverGui == true)
		{
			_prevCursor = CursorManager::Instance()->GetCurrentCursorState();
			CursorManager::Instance()->SetCurrentCursorState("Gui");
		}

		// When F1 is click toggle performance panel visibility
		if( input->CheckKeyWasPressed(OIS::KC_F1) )
		{
			_mainPanel->GetPerformancePanel()->SetVisible(
				!_mainPanel->GetPerformancePanel()->IsVisible());
		}

		// When ~ is click toggle performance panel visibility
		if( input->CheckKeyWasPressed(OIS::KC_GRAVE) )
		{
			if(this->IsConsoleVisible())
				this->HideConsole();
			else
				this->ShowConsole();
		}

		_mainPanel->Update(ms);
	}

	void GuiController::ShowGui()
	{
		GUIManager::Instance()->SetCurrentGUI("GameGui");
		_mainPanel->Show();
	}

	void GuiController::HideGui()
	{
		_mainPanel->Hide();
	}

	void GuiController::ResizeGui(std::pair<int,int> size)
	{
		_mainPanel->Resize(size.first, size.second);
	}

	void GuiController::RegisterCommandButton(CommandButton* button)
	{
		_mainPanel->GetCommandButtonsPanel()->RegisterCommandButton(button);
	}

	void GuiController::ResetCommandButtons()
	{
		_mainPanel->GetCommandButtonsPanel()->ResetButtons();
	}

	Event<CommandButton*>& GuiController::CommandChoosen()
	{
		return _mainPanel->GetCommandButtonsPanel()->CommandChoosen();
	}

	void GuiController::UpdateSelectedUnitGroup(UnitGroup* selectedUnits)
	{
		if(selectedUnits != 0)
		{
			_selectedUnitsCount = selectedUnits->GroupCount();
			if(_selectedUnitsCount == 1)
			{
				// One unit selected -> show unit panel
				GameGui::UnitPanel* upanel = _mainPanel->GetFunctionalPanel<GameGui::UnitPanel>(
					GameGui::FunctionalPanels::UnitInfo);
				_mainPanel->ChangeFunctionalPanel(GameGui::FunctionalPanels::UnitInfo);

				Unit* currUnit = *selectedUnits->GetUnitGroupBegin()->second.begin();
				upanel->SetCurrentUnit(currUnit);
				// Depending on unit state change to proper subpanel
				if(currUnit->IsUnderConstruction())
					upanel->ChangeSubPanel(GameGui::UnitSubPanels::Construction);
				else if(currUnit->CanTrainUnits() && currUnit->IsTrainingUnit() )
					upanel->ChangeSubPanel(GameGui::UnitSubPanels::Training);
				else
					upanel->ChangeSubPanel(GameGui::UnitSubPanels::UnitStats);
			}
			else if(_selectedUnitsCount > 0)
			{
				// Many unit selected -> show selection panel
				_mainPanel->GetFunctionalPanel<GameGui::SelectionPanel>(
					GameGui::FunctionalPanels::SelectedUnits)->
					SetSelectedUnitGroup(selectedUnits);
				_mainPanel->ChangeFunctionalPanel(GameGui::FunctionalPanels::SelectedUnits);
			}
			else if( _mainPanel->GetFunctionalPanel(GameGui::FunctionalPanels::UnitInfo)->IsVisible() ||
				_mainPanel->GetFunctionalPanel(GameGui::FunctionalPanels::SelectedUnits)->IsVisible() )
			{
				// No unit selected, hide selection/unit panel if is visible
				_mainPanel->ResetFunctionalPanel();
			}
		}
	}

	void GuiController::UpdateSelectedUnitContextSubgroup(Array<Unit*>* contextSubgroup)
	{
		auto spanel = _mainPanel->GetFunctionalPanel<GameGui::SelectionPanel>(
			GameGui::FunctionalPanels::SelectedUnits);
		if(spanel->IsVisible())
		{
			spanel->SetContextSubgroup(contextSubgroup);
		}
	}

	Event<Unit*, bool>& GuiController::UnitButtonClicked()
	{
		return _mainPanel->GetFunctionalPanel<GameGui::SelectionPanel>(
			GameGui::FunctionalPanels::SelectedUnits)->ButtonClicked();
	}

	void GuiController::AvailableBuildingsChanged(const Array<TechBuilding*>& newBuildings)
	{
		_mainPanel->GetFunctionalPanel<GameGui::BuildingPanel>(
			GameGui::FunctionalPanels::Buildings)->UpdateBuildingDefinitions(newBuildings);
	}

	Event<TechBuilding*>& GuiController::BuildingButtonClicked()
	{
		return _mainPanel->GetFunctionalPanel<GameGui::BuildingPanel>(
			GameGui::FunctionalPanels::Buildings)->ButtonClicked();
	}

	void GuiController::SwitchFunctionalPanels(int newPanel)
	{
		// If switching to selection, then panel depends on selected units count
		if( newPanel == GameGui::FunctionalPanels::SelectedUnits )
		{
			if(_selectedUnitsCount == 0)
			{
				_mainPanel->ResetFunctionalPanel();
			}
			else if(_selectedUnitsCount == 1)
			{
				_mainPanel->ChangeFunctionalPanel(GameGui::FunctionalPanels::UnitInfo);
			}
			else
				_mainPanel->ChangeFunctionalPanel(GameGui::FunctionalPanels::SelectedUnits);
		}
		else
			_mainPanel->ChangeFunctionalPanel(newPanel);
	}

	bool GuiController::CheckIsCursorOverGui(const Vector2& mpos)
	{
		for(unsigned int i = 0; i < _guiAreas.size(); ++i)
		{
			if( _guiAreas[i].IsHit(mpos) )
				return true;
		}
		return false;
	}

	void GuiController::ParseGuiAreas()
	{
		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		int areasCount = std::stoi( conf->FindOptionValue("Count", "GuiAreas") );
		for(int i = 0; i < areasCount; ++i)
		{
			GuiArea area;
			area.ParseFromConfig( conf->FindOptionValue("Area_" + std::to_string(i+1), "GuiAreas") );
			_guiAreas.push_back(area);
		}
	}

	bool GuiArea::IsHit(int x, int y) const
	{
		return !(x < GetLeft() || x > GetRight() ||
			y < GetTop() || y > GetBottom());
	}

	bool GuiArea::IsHit(const Vector2& pos) const
	{
		return IsHit((int)pos.x, (int)pos.y);
	}

	void GuiArea::ParseFromConfig(const string& areaValue)
	{
		// 4 ints in space separated in order : left, top, width, height
		std::stringstream ss;
		ss << areaValue;
		ss >> Left;
		ss >> Top;
		ss >> Width;
		ss >> Height;
	}

	void GuiController::ShowQuickNotice(const NoticeMessage& msg)
	{
		// Show notices directed to client only
		if( msg.Player == GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer() )
			_mainPanel->GetNotifyPanel()->AddMessage(msg);
	}

	void GuiController::ShowNotice(const NoticeMessage& msg)
	{

	}

	void GuiController::ShowConsole()
	{
		_mainPanel->GetConsolePanel()->Show();
		GuiArea area;
		auto coords = _mainPanel->GetConsolePanel()->GetPanel()->getCoord();
		area.Width = coords.width;
		area.Height = coords.height;
		area.Top = coords.top;
		area.Left = coords.left;
		_guiAreas.push_back(area);
	}

	void GuiController::HideConsole()
	{
		_mainPanel->GetConsolePanel()->Hide();
		_guiAreas.pop_back();
	}

	bool GuiController::IsConsoleVisible()
	{
		return _mainPanel->GetConsolePanel()->IsVisible();
	}

	void GuiController::PrintConsoleOutput(const string& output)
	{
		_mainPanel->GetConsolePanel()->PrintOutput(output);
	}

	Event<Array<string>&>& GuiController::ConsoleQueryAccepted()
	{
		return _mainPanel->GetConsolePanel()->ConsoleQueryAccepted();
	}


	Event<Vector2>& GuiController::MinimapLeftButtonDragged()
	{
		return _mainPanel->GetMinimapPanel()->MinimapLeftButtonDragged();
	}

	Event<Vector2>& GuiController::MinimapLeftButtonClicked()
	{
		return _mainPanel->GetMinimapPanel()->MinimapLeftButtonClicked();
	}

	Event<Vector2>& GuiController::MinimapRightButtonDragged()
	{
		return _mainPanel->GetMinimapPanel()->MinimapRightButtonDragged();
	}

	Event<Vector2>& GuiController::MinimapRightButtonClicked()
	{
		return _mainPanel->GetMinimapPanel()->MinimapRightButtonClicked();
	}
}