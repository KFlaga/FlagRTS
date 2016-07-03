#include "GameGuiFunctionalButtonsPanel.h"
#include <CommandButton.h>
#include <MyGUI\MyGUI.h>
#include "GameGuiIFunctionalPanel.h"
#include <GuiElement.h>
#include <AudioManager.h>


namespace FlagRTS
{
	using namespace GameGui;

	FunctionalButtonsPanel::FunctionalButtonsPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		_onButtonSelectionClicked(this),
		_onButtonBuildingsClicked(this)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "FunctionalPanel"));
		_buttonSelection = xNew3(GuiElement,gameGui, mainLayout, "FunP_SelectionButton");
		_buttonBuildings = xNew3(GuiElement,gameGui, mainLayout, "FunP_BuildingsButton");
		
		_buttonSelection->GetMousePressedEvent() += &_onButtonSelectionClicked;
		_buttonBuildings->GetMousePressedEvent() += &_onButtonBuildingsClicked;

		string pressSound = _buttonSelection->GetMyGUIWidget()->getUserString("PressSound");
		if(pressSound.size() > 0)
		{
			_buttonSelection->SetPressSound(pressSound, "Gui");
		}

		gameGui->AddGuiElement(_buttonSelection);
		gameGui->AddGuiElement(_buttonBuildings);
	}

	FunctionalButtonsPanel::~FunctionalButtonsPanel()
	{
		xDelete(_buttonSelection);
		xDelete(_buttonBuildings);
	}

	void FunctionalButtonsPanel::Show()
	{
		_mainPanel->setVisible(true);
		_buttonSelection->Show();
		_buttonBuildings->Show();
	}

	void FunctionalButtonsPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_buttonSelection->Hide();
		_buttonBuildings->Hide();
	}

	void FunctionalButtonsPanel::OnButtonSelectionClicked(GuiElement* guiButton, int mouseButton)
	{
		if(mouseButton == MouseButtons::Left)
			_functionalPanelSwitchRequested.Fire(FunctionalPanels::SelectedUnits);
	}

	void FunctionalButtonsPanel::OnButtonBuildingsClicked(GuiElement* guiButton, int mouseButton)
	{
		if(mouseButton == MouseButtons::Left)
			_functionalPanelSwitchRequested.Fire(FunctionalPanels::Buildings);
	}

	void FunctionalButtonsPanel::Resize( float ratioHorizontal, float ratioVertical )
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));
	}
}