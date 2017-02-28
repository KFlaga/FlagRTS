#include "OptionsAudio.h"
#include <ConfigurationManager.h>
#include <LogManager.h>
#include <ErrorManager.h>
#include <LocalizationManager.h>
#include <SystemSettings.h>
#include <AudioManager.h>
#include <SliderControl.h>

namespace FlagRTS
{
	OptionsAudio::OptionsAudio(RefPtr<GUI> parentGui) :
		IOptionsPanel(parentGui, parentGui->FindWidget("OptionsMainPanel"), "OptionsAudio"),
		_pendingSettingChanges(false),
		_loaded(false)
	{
		ADD_LOG("Begin Create OptionsAudio");
		// InitGui
		if( _gui->LoadLayout("OptionsAudio.layout", _parentPanel, true) != ErrorCodes::Success)
		{
			ADD_LOG("Couldn't load layout : OptionsAudio.layout");
		}

		// Assign widgets
		RefPtr<Layout> mainLayout = _gui->GetLayout("OptionsAudio.layout");
		//_panel = static_cast<MyGUI::Widget*>(_gui->FindInLayout(mainLayout, "PanelVideo"));
		//_buttonAccept = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonApply"));
		//_labelMaster = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelMasterVolume"));
	
		// Create sliders
		//_sliderMaster = CreateVolumeSlider(parentGui, mainLayout, "MasterVolume");

		// Set event handlers
		//_buttonAccept->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsAudio::ButtonAcceptClicked);

		// Set locale texts
		//_buttonAccept->setCaption(GET_TEXT("ButtonApply","Gui.MainMenu.Options.Video"));

		// Find settings values from configs
		//_newSettings = SystemSettings::GetAudioSettings();

		// Fill controls with data and set current values
		//_sliderMaster->SetCurrentValue( _newSettings.MasterVolume );

		_loaded = true;
		ADD_LOG("End Create OptionsVideo");
	}

	void OptionsAudio::Hide()
	{
		if(_gui.IsValid())
			_gui->HideLayout("OptionsAudio.layout");
	}

	void OptionsAudio::Show()
	{
		if(_gui.IsValid())
			_gui->ShowLayout("OptionsAudio.layout");
	}

	void OptionsAudio::SaveSettings()
	{
		_pendingSettingChanges = false;

		SystemSettings::GetAudioSettings() = _newSettings;
		SystemSettings::GetAudioSettings().SaveSettingsToConfig();
	}

	void OptionsAudio::UndoSettings()
	{
		_newSettings = SystemSettings::GetAudioSettings();
		if(_loaded)
		{

		}
	}

	void OptionsAudio::Unload()
	{
		if(_loaded && _gui.IsValid())
		{
			_loaded = false;
			_gui->UnloadLayout("OptionsAudio.layout");
		}
	}

	ErrorCode OptionsAudio::Update(float ms)
	{
		// If volume changed change it in AudioManager
		if(_pendingSettingChanges)
		{
			_pendingSettingChanges = false;
			Media::AudioManager::Instance()->UpdateSettings();
		}

		return ErrorCodes::Success;
	}

	void OptionsAudio::ButtonAcceptClicked(MyGUI::Widget*)
	{
		SaveSettings();
	}


	Slider3ImagesControl* OptionsAudio::CreateVolumeSlider(
		RefPtr<GUI>& gui, RefPtr<Layout>& layout, const string& name)
	{
		// In layout there should be widgets : name_SliderBG, name_SliderFG, name_SliderValue
		// Slider is created using those widgets
		MyGUI::ImageBox* bg = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(layout, name+"_SliderBG"));
		MyGUI::ImageBox* fg = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(layout, name+"_SliderFG"));
		MyGUI::ImageBox* valImg = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(layout, name+"_SliderValue"));
		Slider3ImagesControl* slider = xNew5(Slider3ImagesControl, bg, fg, valImg, 0.f, 1.f);
		
		return slider;
	}
}