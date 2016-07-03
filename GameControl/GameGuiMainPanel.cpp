#include "GameGuiMainPanel.h"
#include "GameGuiCommandButtonsPanel.h"
#include "GameGuiMenuWindow.h"
#include "GameGuiMinimapPanel.h"
#include "GameGuiSelectionPanel.h"
#include "GameGuiUnitPanel.h"
#include "GameGuiPerformancePanel.h"
#include "GameGuiResourcesPanel.h"
#include "GameGuiBuildingPanel.h"
#include "GameGuiFunctionalButtonsPanel.h"
#include "GameGuiNotifyPanel.h"
#include "GameGuiConsole.h"
#include <MyGUI\MyGUI.h>

#include <RenderSystemManager.h>
#include <DataTypes.h>
#include <Memory.h>
#include <GUIManager.h>
#include <InputManager.h>
#include <ConfigurationManager.h>
#include <Exception.h>

namespace FlagRTS
{
	using namespace GameGui;

	MainPanel::MainPanel() :
		_gui(xNew1(GUI,"GameGui")),
		_mainPanel(0),
		_menuWindow(0),
		_perfPanel(0),
		_bottomPanel(0),
		_buttonsPanel(0),
		_minimapPanel(0),
		_resourcesPanel(0),
		_currentFuncPanel(0),
		_notifyPanel(0),
		_consolePanel(0)
	{
		CreateGui();
	}

	MainPanel::~MainPanel()
	{
		GUIManager::Instance()->UnregisterGUI("GameGui");
		xDeleteSafe(_menuWindow);
		xDeleteSafe(_perfPanel);
		for(unsigned int i = 0; i < _funcPanels.size(); ++i)
		{
			xDeleteSafe(_funcPanels[i]);
		}
		xDeleteSafe(_consolePanel);
		xDeleteSafe(_notifyPanel);
		xDeleteSafe(_buttonsPanel);
		xDeleteSafe(_minimapPanel);
		xDeleteSafe(_resourcesPanel);
		xDeleteSafe(_funcButtonsPanel);
	}

	void MainPanel::Show()
	{
		_mainPanel->setVisible(true);
		_perfPanel->SetVisible(true);
		_bottomPanel->setVisible(true);
		_resourcesPanel->Show();
		_buttonsPanel->Show();
		_funcButtonsPanel->Show();
		_notifyPanel->Show();
		_minimapPanel->Show();
		if(_currentFuncPanel != 0)
			_currentFuncPanel->Show();
	}

	void MainPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_perfPanel->SetVisible(false);
		_bottomPanel->setVisible(false);
		_buttonsPanel->Hide();
		_resourcesPanel->Hide();
		_funcButtonsPanel->Hide();
		_notifyPanel->Hide();
		_consolePanel->Hide();
		_minimapPanel->Hide();
		if(_currentFuncPanel != 0)
			_currentFuncPanel->Hide();
	}


	void MainPanel::Update(float ms)
	{
		_perfPanel->Update(ms);
		if(_currentFuncPanel != 0 && _currentFuncPanel->IsVisible())
			_currentFuncPanel->Update(ms);
		_resourcesPanel->Update(ms);
		_notifyPanel->Update(ms);
		_minimapPanel->Update(ms);
	}

	void MainPanel::CreateGui()
	{
		int screenWidth = RenderSystemManager::Instance()->GetMainWindow()->getWidth();
		int screenHeight = RenderSystemManager::Instance()->GetMainWindow()->getHeight();

		_gui->LoadLayout("GameGui.layout", 0, true);

		RefPtr<Layout> mainLayout = _gui->GetLayout("GameGui.layout");
		_mainPanel = static_cast<MyGUI::Widget*>(_gui->FindInLayout(mainLayout, "MainPanel"));
		_bottomPanel = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(mainLayout, "BottomPanel"));

		GUIManager::Instance()->RegisterGUI("GameGui", _gui); 

		_buttonsPanel = xNew2(CommandButtonsPanel, _gui, mainLayout);
		_resourcesPanel = xNew2(ResourcesPanel, _gui, mainLayout);
		_funcButtonsPanel = xNew2(FunctionalButtonsPanel, _gui, mainLayout);

		_funcPanels.resize(FunctionalPanels::FP_PanelsCount);
		_funcPanels[FunctionalPanels::UnitInfo] = xNew2(UnitPanel, _gui, mainLayout);
		_funcPanels[FunctionalPanels::SelectedUnits] = xNew2(SelectionPanel, _gui, mainLayout);
		_funcPanels[FunctionalPanels::Buildings] = xNew2(BuildingPanel, _gui, mainLayout);
		
		_minimapPanel = xNew2(MinimapPanel, _gui, mainLayout);
		_perfPanel = xNew1(PerformancePanel, _mainPanel);
		_notifyPanel = xNew2(NotifyPanel, _gui, mainLayout);
		_consolePanel =  xNew1(ConsolePanel, _mainPanel);
	}

	void MainPanel::ResetFunctionalPanel()
	{
		if(_currentFuncPanel != 0)
			_currentFuncPanel->Hide();
		_currentFuncPanel = 0;
	}

	void MainPanel::ChangeFunctionalPanel(int type)
	{
		IFunctionalPanel* fpanel = _funcPanels[type];
		if(fpanel == _currentFuncPanel)
			return;

		if(_currentFuncPanel != 0)
			_currentFuncPanel->Hide();
		_currentFuncPanel = fpanel;
		_currentFuncPanel->Show();
	}

	void MainPanel::Resize(int width, int height)
	{
		float ratioHorizontal = (float)width / _mainPanel->getAbsoluteCoord().width;
		float ratioVertical = (float)height /_mainPanel->getAbsoluteCoord().height;
		_mainPanel->setCoord(0, 0, width, height);
		
		const MyGUI::IntCoord& oldCoord = _bottomPanel->getCoord();
		_bottomPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		_buttonsPanel->Resize(ratioHorizontal, ratioVertical);
		_resourcesPanel->Resize(ratioHorizontal, ratioVertical);

		for(unsigned int i = 0; i < _funcPanels.size(); ++i)
		{
			_funcPanels[i]->Resize(ratioHorizontal, ratioVertical);
		}

		float perfPanelWidth = 0.25f;
		float perfPanelHeight = 0.2f;
		float perfPanelTop = 0.15f;
		float perfPanelLeft = 0.75f;

		_perfPanel->GetMainWidget()->setRealCoord(
			perfPanelLeft, perfPanelTop,
			perfPanelWidth, perfPanelHeight);

		_notifyPanel->Resize(ratioHorizontal, ratioVertical);
		_consolePanel->Resize(ratioHorizontal, ratioVertical);
		_minimapPanel->Resize(ratioHorizontal, ratioVertical);
	}
}