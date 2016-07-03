#pragma once

#include <GUIManager.h>
#include "OptionsPanel.h"
#include <AudioSettings.h>

namespace FlagRTS
{
	class Slider3ImagesControl;

	class OptionsAudio : public IOptionsPanel
	{
	private:
		AudioSettings _newSettings;
		MyGUI::Button* _buttonAccept;
		
		MyGUI::EditBox* _labelMaster;
		MyGUI::EditBox* _labelMusic;
		MyGUI::EditBox* _labelGui;
		MyGUI::EditBox* _labelAlerts;
		MyGUI::EditBox* _labelRespones;
		MyGUI::EditBox* _labelGameSounds;
		Slider3ImagesControl* _sliderMaster;

		MyGUI::Widget* _panel;
		bool _loaded;
		bool _pendingSettingChanges;

	public:
		OptionsAudio(RefPtr<GUI> parentGui);

		void Hide();
		void Show();
		void SaveSettings();
		void UndoSettings();
		void Unload();

		ErrorCode Update(float ms);

	private:
		void ButtonAcceptClicked(MyGUI::Widget*);

		Slider3ImagesControl* CreateVolumeSlider(RefPtr<GUI>& gui, RefPtr<Layout>& layout, const string& name);
	};
}