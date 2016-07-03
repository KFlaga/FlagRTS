#pragma once

#include <Event.h>
#include <GUI.h>
#include <GuiElement.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class Button;
}

namespace FlagRTS
{
	class CommandButton;

	namespace GameGui
	{
		// Class representing cell 3x4 of buttons, which on press chooses bind command to be ordered
		// ( notifies listener that command was choosen )
		// With change of unit selected unit group panel should be reset and group command buttons
		// should be registered
		class CommandButtonsPanel
		{
		private:
			MyGUI::ImageBox* _buttonPanel;
			GuiElement* _buttons[3][4];
			int _offX;
			int _offY;
			int _bsizeX;
			int _bsizeY;
			int _bmarginX;
			int _bmarginY;

			Event<CommandButton*> _onCommandChoosen;

		public:
			CommandButtonsPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~CommandButtonsPanel();

			// Adds command button to panel : loads its icon and binds press event
			void RegisterCommandButton(CommandButton* command);
			// Removes all icons and unregisters press events
			void ResetButtons();

			Event<CommandButton*>& CommandChoosen() { return _onCommandChoosen; }

			GuiElement* GetButton(unsigned int x, unsigned int y) const { return _buttons[y][x]; }
			MyGUI::Button* GetMyGUIButton(unsigned int x, unsigned int y) const 
			{ 
				return _buttons[y][x]->GetMyGUIWidget<MyGUI::Button>(); 
			}

			MyGUI::ImageBox* GetPanel() const { return _buttonPanel; }

			void Show();
			void Hide();

			void Resize( float ratioHorizontal, float ratioVertical );

		private:
			void OnButtonClicked(GuiElement*, int);
			DEFINE_DELEGATE2(ButtonClickedDelegate, CommandButtonsPanel,
				OnButtonClicked, GuiElement*, int);
			ButtonClickedDelegate _onButtonClicked;
		};
	}
}