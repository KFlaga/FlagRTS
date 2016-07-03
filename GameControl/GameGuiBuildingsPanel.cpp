#include "GameGuiBuildingPanel.h"
#include <MyGUI\MyGUI.h>
#include <DataTypes.h>
#include <UnitDefinition.h>
#include <ConfigurationManager.h>
#include <Technology.h>
#include <Exception.h>

namespace FlagRTS
{
	using namespace GameGui;

	BuildingPanel::BuildingPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		IFunctionalPanel(FunctionalPanels::SelectedUnits),
		_firstMilitBuilding(0),
		_firstEcoBuilding(0)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "BuildingsPanel"));
		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		_buttonsCols = std::stoi(conf->FindOptionValue("Count_X", "BuildingsPanel.Buttons"));
		_buttonsRows = 2;
		_buttons.resize(_buttonsCols*_buttonsRows);
		_offX = std::stoi(conf->FindOptionValue("Offset_X", "BuildingsPanel.Buttons"));
		_offY = std::stoi(conf->FindOptionValue("Offset_Y", "BuildingsPanel.Buttons"));
		_bsizeX = std::stoi(conf->FindOptionValue("Size_X", "BuildingsPanel.Buttons"));
		_bsizeY = std::stoi(conf->FindOptionValue("Size_Y", "BuildingsPanel.Buttons"));
		_bmarginX = std::stoi(conf->FindOptionValue("Margin_X", "BuildingsPanel.Buttons"));
		_bmarginY = std::stoi(conf->FindOptionValue("Margin_Y", "BuildingsPanel.Buttons"));

		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			_buttons[but] = xNew1(ConstructionButton, _mainPanel);
			_buttons[but]->GetButton()->eventMouseButtonClick += 
				MyGUI::newDelegate(this, &BuildingPanel::OnButtonPressed);
			_buttons[but]->GetButton()->setUserData(_buttons[but]);
		}

		_nextPageEconomyButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(mainLayout, "BuildP_NextEco"));
		_nextPageEconomyButton->eventMouseButtonClick += 
			MyGUI::newDelegate(this, &BuildingPanel::OnNextEcoPageClicked);
		_prevoiusEconomyPageButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(mainLayout, "BuildP_PrevEco"));
		_prevoiusEconomyPageButton->eventMouseButtonClick += 
			MyGUI::newDelegate(this, &BuildingPanel::OnPrevEcoPageClicked);
		_nextPageMilitaryButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(mainLayout, "BuildP_NextMilit"));
		_nextPageMilitaryButton->eventMouseButtonClick += 
			MyGUI::newDelegate(this, &BuildingPanel::OnNextMilitPageClicked);
		_prevoiusMilitaryPageButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(mainLayout, "BuildP_PrevMilit"));
		_prevoiusMilitaryPageButton->eventMouseButtonClick += 
			MyGUI::newDelegate(this, &BuildingPanel::OnPrevMilitPageClicked);

		_mainPanel->setVisible(false);
	}

	BuildingPanel::~BuildingPanel()
	{
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			xDelete(_buttons[but]);
		}
	}

	void BuildingPanel::Show()
	{
		_isVisible = true;
		_mainPanel->setVisible(true);
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			if(_buttons[but]->GetBuilding() != 0)
				_buttons[but]->Show();
		}
	}

	void BuildingPanel::Hide()
	{
		_isVisible = false;
		_mainPanel->setVisible(false);
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			if(_buttons[but]->GetBuilding() != 0)
				_buttons[but]->Hide();
		}
	}

	void BuildingPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		int buttonWidth = (int)(_bsizeX * ratioHorizontal);
		int buttonHeight = (int)(_bsizeY * ratioVertical);
		int offsetX = (int)(_offX * ratioHorizontal);
		int offsetY = (int)(_offY * ratioHorizontal);
		int marginX = (int)(_bmarginX * ratioHorizontal);
		int marginY = (int)(_bmarginY * ratioVertical);
		int shiftX = buttonWidth + marginX;
		int shiftY = buttonHeight + marginY;
		for(int y = 0; y < _buttonsRows; ++y)
		{
			for(int x = 0; x < _buttonsCols; ++x)
			{
				_buttons[x+y*_buttonsCols]->Resize( 
					offsetX + shiftX*x , offsetY + shiftY*y, 
					buttonWidth, buttonHeight );
			}
		}
	}

	void BuildingPanel::UpdateBuildingDefinitions(const Array<TechBuilding*>& newBuildings)
	{
		_buildings = newBuildings;
		// Look in buildings array
		// for each if its eco add to next eco button, else to Milit
		// omit first few if _firstEcoBuilding/milit != 0
		int ecosToLeave = _firstEcoBuilding, militsToLeave = _firstMilitBuilding;
		int currEcoBut = 0, currMilitBut = 0;
		for(unsigned int i = 0; i < _buildings.size(); ++i)
		{
			switch( _buildings[i]->Family )
			{
			case TechBuildingFamily::Economy:
				if( ecosToLeave > 0)
				{
					--ecosToLeave;
					continue;
				}
				if(currEcoBut >= _buttonsCols)
					continue;
				GetButton(currEcoBut, 0)->SetBuilding(_buildings[i]);
				GetButton(currEcoBut, 0)->SetConstructionEnabled(
					_buildings[i]->Availability == TechAvailability::Available );
				++currEcoBut;
				break;
			case TechBuildingFamily::Military:
				if( militsToLeave > 0)
				{
					--militsToLeave;
					continue;
				}
				if(currMilitBut >= _buttonsCols)
					continue;
				GetButton(currMilitBut, 1)->SetBuilding(_buildings[i]);
				GetButton(currMilitBut, 1)->SetConstructionEnabled(
					_buildings[i]->Availability == TechAvailability::Available );
				++currMilitBut;
				break;
			default:
				CastException_d("Bad building tech family");
			}
		}
	}


	void BuildingPanel::OnButtonPressed(MyGUI::Widget* button)
	{
		ConstructionButton* cbutton = *button->getUserData<ConstructionButton*>();
		if( cbutton->GetBuilding() != 0 )
			_buttonClicked.Fire(cbutton->GetBuilding()); 
	}

	void BuildingPanel::OnNextEcoPageClicked(MyGUI::Widget* button)
	{

	}

	void BuildingPanel::OnPrevEcoPageClicked(MyGUI::Widget* button)
	{

	}

	void BuildingPanel::OnNextMilitPageClicked(MyGUI::Widget* button)
	{

	}

	void BuildingPanel::OnPrevMilitPageClicked(MyGUI::Widget* button)
	{

	}
}