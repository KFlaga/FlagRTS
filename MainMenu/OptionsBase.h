#pragma once

#include <GUI.h>
#include "IMenuScreen.h"
#include "OptionsPanel.h"
#include "StringUtils.h"

namespace FlagRTS
{
	class OptionsBase : public IMenuScreen
	{
		private:
		MyGUI::ImageBox* _bgPanel; // Backgroung panel - should have bg inmage loaded
		MyGUI::Button* _buttonVideo;
		MyGUI::Button* _buttonAudio;
		MyGUI::Button* _buttonInput;
		MyGUI::Button* _buttonGame;
		MyGUI::Button* _buttonReturn;
		std::map<string, IOptionsPanel*, StringLess> _optionsPanels;
		IOptionsPanel* _currentOptionsPanel;
		bool _loaded; // True if gui is loaded
		bool _pendingReturnToMenu;

	public:
		OptionsBase();
		~OptionsBase();

		// Updates state of menu ( pool pending close/load etc requests which shouldn't be done
		// synchronously )
		ErrorCode Update(float ms);
		// Unloads gui
		void Unload();
		// Show gui
		void Show();
		// Returns true if request to back to previous menu is pending
		bool IsReturnPending() const { return _pendingReturnToMenu; }

		void Resize(int width, int height);

	private:
		IOptionsPanel* FindOptionsPanel(const string& name)
		{
			return _optionsPanels.find(name)->second;
		}

		// Callback for video button. Changes current panel to OptionsVideo
		void ChangeOptionTabToVideo(MyGUI::Widget*);
		// Callback for audio button. Changes current panel to OptionsAudi
		void ChangeOptionTabToAudio(MyGUI::Widget*);
		// Callback for input button. Changes current panel to OptionsInput
		void ChangeOptionTabToInput(MyGUI::Widget*);
		// Callback for game button. Changes current panel to OptionsGame
		void ChangeOptionTabToGame(MyGUI::Widget*);
		// Callback for return button. Returns to MainMenu ( Changes current gui )
		void ReturnToMenu(MyGUI::Widget*);
	};
}