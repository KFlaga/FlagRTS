#include "GameGuiCommandButtonsPanel.h"
#include <CommandButton.h>
#include <MyGUI\MyGUI.h>
#include <ConfigurationManager.h>


namespace FlagRTS
{
	using namespace GameGui;

	CommandButtonsPanel::CommandButtonsPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		_onButtonClicked(this)
	{
		_buttonPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "ButtonsPanel"));

		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		_offX = std::stoi(conf->FindOptionValue("Offset_X", "ButtonsPanel.Buttons"));
		_offY = std::stoi(conf->FindOptionValue("Offset_Y", "ButtonsPanel.Buttons"));
		_bsizeX = std::stoi(conf->FindOptionValue("Size_X", "ButtonsPanel.Buttons"));
		_bsizeY = std::stoi(conf->FindOptionValue("Size_Y", "ButtonsPanel.Buttons"));
		_bmarginX = std::stoi(conf->FindOptionValue("Margin_X", "ButtonsPanel.Buttons"));
		_bmarginY = std::stoi(conf->FindOptionValue("Margin_Y", "ButtonsPanel.Buttons"));

		//_defaultIcon = "Icon_ButtonDefault";
		// Create 12 buttons
		for(int y = 0; y < 3; ++y)
		{
			for(int x = 0; x < 4; ++x)
			{
				string name("CommandButton" + std::to_string(y*4+x));
				MyGUI::Button* button =  _buttonPanel->createWidget<MyGUI::Button>("ButtonImage",
					_offX + (_bsizeX + _bmarginX)*x, _offY + (_bsizeY + _bmarginY)*y, 
					_bsizeX, _bsizeY, MyGUI::Align::Default, name);

				_buttons[y][x] = xNew1(GuiElement, button);
				_buttons[y][x]->GetMouseClickedEvent() += &_onButtonClicked;
				_buttons[y][x]->Enable();

				button->setImageResource("NoCommandIcon");
				button->setModeImage(true);

				gameGui->AddGuiElement(_buttons[y][x]);
			}
		}
	}

	CommandButtonsPanel::~CommandButtonsPanel()
	{
		for(int y = 0; y < 3; ++y)
		{
			for(int x = 0; x < 4; ++x)
			{
				xDelete(_buttons[y][x]);
			}
		}
	}

	void CommandButtonsPanel::RegisterCommandButton(CommandButton* command)
	{
		// Set command icon and button pressed event
		GuiElement* button = GetButton(command->GetPosX(),command->GetPosY());
		button->GetMyGUIWidget<MyGUI::Button>()->setImageResource(command->GetIconName());
		button->SetUserData(command);
		button->Enable();
	}

	void CommandButtonsPanel::ResetButtons()
	{
		for(int y = 0; y < 3; ++y)
		{
			for(int x = 0; x < 4; ++x)
			{
				// Set default icons / disable buttons
				GetMyGUIButton(x, y)->setImageResource("NoCommandIcon");
				GetButton(x, y)->Disable();
			}
		}
	}

	void CommandButtonsPanel::Show()
	{
		_buttonPanel->setVisible(true);
	}

	void CommandButtonsPanel::Hide()
	{
		_buttonPanel->setVisible(false);
	}

	void CommandButtonsPanel::OnButtonClicked(GuiElement* guibutton, int mouseButton)
	{
		if( mouseButton == MouseButtons::Left )
		{
			try
			{
				_onCommandChoosen.Fire(*guibutton->GetUserData<CommandButton*>());
			}
			catch(...)
			{ }
		}
	}

	void CommandButtonsPanel::Resize( float ratioHorizontal, float ratioVertical )
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		/*int buttonWidth = (int)(_bsizeX * ratioHorizontal);
		int buttonHeight = (int)(_bsizeY * ratioVertical);
		int offsetX = (int)(_offX * ratioHorizontal);
		int offsetY = (int)(_offY * ratioHorizontal);
		int marginX = (int)(_bmarginX * ratioHorizontal);
		int marginY = (int)(_bmarginY * ratioVertical);
		int shiftX = buttonWidth + marginX;
		int shiftY = buttonHeight + marginY;*/
		for(int y = 0; y < 3; ++y)
		{
			for(int x = 0; x < 4; ++x)
			{
				GetButton(x,y)->Resize(ratioHorizontal, ratioVertical);
			}
		}
	}
}