#include "GameGuiSelectionPanel.h"
#include <MyGUI\MyGUI.h>
#include <UnitGroup.h>
#include <DataTypes.h>
#include <Unit.h>
#include <ConfigurationManager.h>

namespace FlagRTS
{
	using namespace GameGui;

	SelectionPanel::SelectionPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		IFunctionalPanel(FunctionalPanels::SelectedUnits),
		_selectedUnits(0),
		_activeButtonPage(0),
		_contextSubgroupType(0)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "SelectionPanel"));
		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		_buttonsCols = std::stoi(conf->FindOptionValue("Count_X", "SelectionPanel.Buttons"));
		_buttonsRows = std::stoi(conf->FindOptionValue("Count_Y", "SelectionPanel.Buttons"));
		_buttons.resize(_buttonsCols*_buttonsRows);
		_offX = std::stoi(conf->FindOptionValue("Offset_X", "SelectionPanel.Buttons"));
		_offY = std::stoi(conf->FindOptionValue("Offset_Y", "SelectionPanel.Buttons"));
		_frameX = std::stoi(conf->FindOptionValue("Frame_X", "SelectionPanel.Buttons"));
		_frameY = std::stoi(conf->FindOptionValue("Frame_Y", "SelectionPanel.Buttons"));
		_bsizeX = std::stoi(conf->FindOptionValue("Size_X", "SelectionPanel.Buttons"));
		_bsizeY = std::stoi(conf->FindOptionValue("Size_Y", "SelectionPanel.Buttons"));
		_bmarginX = std::stoi(conf->FindOptionValue("Margin_X", "SelectionPanel.Buttons"));
		_bmarginY = std::stoi(conf->FindOptionValue("Margin_Y", "SelectionPanel.Buttons"));

		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			_buttons[but] = xNew1(UnitButton, _mainPanel);
			_buttons[but]->GetButton()->eventMouseButtonClick += 
				MyGUI::newDelegate(this, &SelectionPanel::OnButtonPressed);
			_buttons[but]->GetButton()->setUserData(_buttons[but]);
		}
		_mainPanel->setVisible(false);
	}

	SelectionPanel::~SelectionPanel()
	{
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			xDelete(_buttons[but]);
		}
	}

	void SelectionPanel::Show()
	{
		_isVisible = true;
		_mainPanel->setVisible(true);
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			if(_buttons[but]->GetUnit() != 0)
				_buttons[but]->Show();
		}
	}

	void SelectionPanel::Hide()
	{
		_isVisible = false;
		_mainPanel->setVisible(false);
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			if(_buttons[but]->GetUnit() != 0)
				_buttons[but]->Hide();
		}
	}

	void SelectionPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));
		
		int buttonWidth = (int)(_bsizeX * ratioHorizontal);
		int buttonHeight = (int)(_bsizeY * ratioVertical);
		int offsetX = (int)(_offX * ratioHorizontal);
		int offsetY = (int)(_offY * ratioHorizontal);
		int frameWidth = (int)(_frameX * ratioHorizontal);
		int frameHeight = (int)(_frameY * ratioVertical);
		int marginX = (int)(_bmarginX * ratioHorizontal);
		int marginY = (int)(_bmarginY * ratioVertical);
		int shiftX = buttonWidth + 2*frameWidth + marginX;
		int shiftY = buttonHeight + 2*frameHeight + marginY;
		for(int y = 0; y < _buttonsRows; ++y)
		{
			for(int x = 0; x < _buttonsCols; ++x)
			{
				_buttons[x+y*_buttonsCols]->Resize( offsetX + shiftX*x , offsetY + shiftY*y, 
					buttonWidth, buttonHeight, frameWidth, frameHeight );
			}
		}
	}

	void SelectionPanel::SetSelectedUnitGroup(UnitGroup* group)
	{
		_selectedUnits = group;
		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			_buttons[but]->SetUnit(0);
			_buttons[but]->DisableHighlight();
		}
		// Set units in buttons and show them
		if(group != 0)
		{
			// Bind first 16 units to buttons
			_activeButtonPage = 0;
			BindUnitsToButtons();
		}
	}

	void SelectionPanel::SetContextSubgroup(Array<Unit*>* subgroup)
	{
		// Highlight unitbuttons with units from this subgroup
		if(subgroup != 0)
		{
			_contextSubgroupType = (*subgroup->begin())->GetUnitKind();
			for(unsigned int but = 0; but < _buttons.size(); ++but)
			{
				if( _buttons[but]->GetUnit() == 0 )
					break;
				if(_buttons[but]->GetUnit()->GetUnitKind() == _contextSubgroupType)
					_buttons[but]->EnableHighlight();
				else if( _buttons[but]->IsHighlighted() )
					_buttons[but]->DisableHighlight();
			}
		}
		else
		{
			for(unsigned int but = 0; but < _buttons.size(); ++but)
			{
				if( _buttons[but]->IsHighlighted() )
					_buttons[but]->DisableHighlight();
			}
		}
	}


	void SelectionPanel::BindUnitsToButtons()
	{
		// There will be 2 rows of buttons 
		// Additional units can will be "below" those 2 rows
		// so when switchng pages, bottow 8 units will be bind to upper buttons
		// This function will only bind first 16 units to their buttons
		auto currUnitTypeIt = _selectedUnits->GetUnitGroupBegin(); // Current unit type list
		auto groupEnd = _selectedUnits->GetUnitGroupEnd(); // End of unit group
		auto currUnitIt = currUnitTypeIt->second.begin(); // Current unit iterator
		auto currTypeEnd = currUnitTypeIt->second.end(); // End of current unit type list

		for(unsigned int but = 0; but < _buttons.size(); ++but)
		{
			// If units of current type ended, switch to next type
			if(currUnitIt == currTypeEnd)
			{
				// If units from entire group ended, end addition of buttons
				++currUnitTypeIt;
				if(currUnitTypeIt == groupEnd)
					return;
				currUnitIt = currUnitTypeIt->second.begin();
				currTypeEnd = currUnitTypeIt->second.end();
			}
			_buttons[but]->SetUnit(*currUnitIt);
			++currUnitIt;
		}
	}


	void SelectionPanel::OnButtonPressed(MyGUI::Widget* button)
	{
		UnitButton* ubutton = *button->getUserData<UnitButton*>();
		_buttonClicked.Fire(ubutton->GetUnit(), ubutton->IsHighlighted()); 
 	}

	void SelectionPanel::NextButtonPage()
	{

	}

	void SelectionPanel::PreviousButtonPage()
	{

	}
}