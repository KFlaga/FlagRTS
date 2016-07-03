#include "GameGuiConsole.h"
#include <MyGUI\MyGUI.h>
#include <sstream>

namespace FlagRTS
{
	using namespace GameGui;

	ConsolePanel::ConsolePanel(MyGUI::Widget* parent)
	{
		if(parent != 0)
		{
			_mainPanel = parent->createWidgetReal<MyGUI::Widget>("WhiteSkin",
				0.f, 0.f, 1.f, 0.33f, MyGUI::Align::Right, "ConsolePanel");
		}
		else
		{
			_mainPanel = MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::Widget>("WhiteSkin",
				0.f, 0.f, 1.f, 0.33f, MyGUI::Align::Right, "ConsolePanel");
		}
		_mainPanel->setInheritsAlpha(false);
		_mainPanel->setColour(MyGUI::Colour(0.5f,0.5f,0.5f,0.5f));
		_mainPanel->setAlpha(0.5f);
		_mainPanel->setPointer("Cursor_Default");

		_consoleOutput = _mainPanel->createWidgetReal<MyGUI::EditBox>("TextBox",
			0.01f, 0.01f, 0.98f, 0.88f, MyGUI::Align::Right, "ConsolePanel");
		_consoleOutput->setTextAlign(MyGUI::Align::Left | MyGUI::Align::Bottom); 
		_consoleOutput->setTextColour(MyGUI::Colour::White);
		_consoleOutput->setEditMultiLine(true);
		_consoleOutput->setEditReadOnly(true);
		_consoleOutput->setFontHeight(13);
		_consoleOutput->setMaxTextLength(10000000);
		_consoleOutput->setVisibleHScroll(true);
		_consoleOutput->setVisibleVScroll(true);
		_consoleOutput->setPointer("Cursor_Text");

		_consoleInput = _mainPanel->createWidgetReal<MyGUI::EditBox>("EditBox",
			0.01f, 0.90f, 0.98f, 0.09f, MyGUI::Align::Right, "ConsolePanel");
		_consoleInput->setTextAlign(MyGUI::Align::Left); 
		_consoleInput->setTextColour(MyGUI::Colour::Black);
		_consoleInput->setEditReadOnly(false);
		_consoleInput->setEnabled(true);
		_consoleInput->setFontHeight(13);
		_consoleInput->setMaxTextLength(1000);
		_consoleInput->setVisibleHScroll(false);
		_consoleInput->setVisibleVScroll(false);
		_consoleInput->setOverflowToTheLeft(true);
		_consoleInput->setPointer("Cursor_Text");

		_consoleInput->eventKeyButtonPressed += MyGUI::newDelegate(this, &ConsolePanel::KeyPressed);

		_mainPanel->setVisible(false);
	}

	void ConsolePanel::Show()
	{
		_mainPanel->setVisible(true);
	}

	void ConsolePanel::Hide()
	{
		_mainPanel->setVisible(false);
	}

	bool ConsolePanel::IsVisible()
	{
		return _mainPanel->isVisible();
	}

	void ConsolePanel::PrintOutput(const string& output)
	{
		string newOutput = _consoleOutput->getCaption() + output + "\r\n";
		_consoleOutput->setCaption(_consoleOutput->getCaption() + output);
	}

	void ConsolePanel::ClearOutput()
	{
		_consoleOutput->setCaption("");
	}

	void ConsolePanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		GetPanel()->setRealCoord(0.f,0.f,1.f,0.33f);
		_consoleOutput->setRealCoord(0.01f, 0.01f, 0.98f, 0.88f);
		_consoleInput->setRealCoord(0.01f, 0.90f, 0.98f, 0.09f);
	}

	void ConsolePanel::KeyPressed(MyGUI::Widget* widget, MyGUI::KeyCode keyCode, unsigned int charCode)
	{
		if(_mainPanel->getVisible() == false || charCode == '~')
			return;
		
		if( keyCode == MyGUI::KeyCode::Return )
		{
			// Accept console query if: its not clean and size > 0
			string input = _consoleInput->getCaption();
			if(input.size() == 0)
				return;

			std::stringstream qstream;
			qstream << input;

			_lastQuery.clear();
			// Split query by whitespaces
			while(!qstream.eof())
			{
				string word;
				qstream >> word;
				if( word.size() > 0 )
					_lastQuery.push_back(word);
			}

			if(_lastQuery.size() > 0)
			{
				if(_lastQuery[0].compare("clean"))
					ClearOutput();
				else
					_consoleQueryAccepted.Fire(_lastQuery);
			}
			_consoleInput->setCaption("");
		}
	}
}