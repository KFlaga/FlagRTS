#pragma once

#include <GUIManager.h>
#include "OptionsPanel.h"
#include <VideoSettings.h>

namespace FlagRTS
{
	class OptionsVideo : public IOptionsPanel
	{
	private:
		VideoSettings _newSettings;
		MyGUI::Button* _buttonAccept;
		
		MyGUI::EditBox* _labelResoultion;
		MyGUI::ComboBox* _comboResoultion;
		MyGUI::EditBox* _labelRenderer;
		MyGUI::ComboBox* _comboRenderer;
		MyGUI::Button* _checkFullscreen;
		MyGUI::EditBox* _labelFullscreen;
		MyGUI::Button* _checkVSync;
		MyGUI::EditBox* _labelVSync;

		MyGUI::Button* _buttonFSAALeft;
		MyGUI::Button* _buttonFSAARight;
		MyGUI::EditBox* _valueFSAA;
		MyGUI::EditBox* _labelFSAA;
		Array<int> _availableFSAAs;
		int _currentFSAAIdx;

		MyGUI::Widget* _panel;
		bool _loaded;
		bool _needRestartForChanges;
		bool _pendingSettingChanges;

	public:
		OptionsVideo(RefPtr<GUI> parentGui);

		void Hide();
		void Show();
		void SaveSettings();
		void UndoSettings();
		void Unload();

		ErrorCode Update(float ms);

	private:
		void ButtonAcceptClicked(MyGUI::Widget*);
		void ButtonFullscreenClicked(MyGUI::Widget*);
		void ButtonVSyncClicked(MyGUI::Widget*);
		void ButtonFSAALeftClicked(MyGUI::Widget*);
		void ButtonFSAARightClicked(MyGUI::Widget*);
		void ComboResolutionChanged(MyGUI::ComboBox* sender, size_t index);
		void ComboRendererChanged(MyGUI::ComboBox* sender, size_t index);

		void AcceptSettings(MyGUI::Widget*);

		void VideoModeChanged(MyGUI::ComboBox* sender, size_t index);

		void FullscreenChanged(MyGUI::Widget*);
	};
}