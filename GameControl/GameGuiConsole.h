#pragma once

#include <Event.h>
#include <DataTypes.h>
#include <MyGUI\MyGUI_KeyCode.h>
#include <Array.h>

namespace MyGUI
{
	class Widget;
	class ScrollView;
	class TextBox;
	class EditBox;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class ConsolePanel
		{
		protected:
			MyGUI::Widget* _mainPanel;
			MyGUI::EditBox* _consoleOutput;
			MyGUI::EditBox* _consoleInput;
			Event<Array<string>&> _consoleQueryAccepted;
			Array<string> _lastQuery;

		public:
			ConsolePanel(MyGUI::Widget* parent = 0);

			void Show();
			void Hide();
			bool IsVisible();

			MyGUI::Widget* GetPanel() { return _mainPanel; }

			void PrintOutput(const string& output);

			Event<Array<string>&>& ConsoleQueryAccepted() { return _consoleQueryAccepted; }

			void Resize(float ratioHorizontal, float ratioVertical);

			void ClearOutput();

		protected:
			void KeyPressed(MyGUI::Widget* widget, MyGUI::KeyCode keyCode, unsigned int charCode);
		};
	}
}