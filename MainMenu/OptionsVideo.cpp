#include "OptionsVideo.h"
#include <ConfigurationManager.h>
#include <LogManager.h>
#include <ErrorManager.h>
#include <RenderSystemManager.h>
#include <LocalizationManager.h>
#include <SystemSettings.h>

namespace FlagRTS
{
	OptionsVideo::OptionsVideo(RefPtr<GUI> parentGui) :
		IOptionsPanel(parentGui, parentGui->FindWidget("OptionsMainPanel"), "OptionsVideo"),
		_needRestartForChanges(false), 
		_pendingSettingChanges(false),
		_loaded(false)
	{
		ADD_LOG("Begin Create OptionsVideo");
		// InitGui
		if( _gui->LoadLayout("OptionsVideo.layout", _parentPanel, true) != ErrorCodes::Success)
		{
			ADD_LOG("Couldn't load layout : OptionsVideo.layout");
		}

		// Assign widgets
		RefPtr<Layout> mainLayout = _gui->GetLayout("OptionsVideo.layout");
		_panel = static_cast<MyGUI::Widget*>(_gui->FindInLayout(mainLayout, "PanelVideo"));
		_buttonAccept = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonApply"));

		_labelResoultion = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelResolution"));
		_comboResoultion = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboResolution"));
		_labelRenderer = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelRenderer"));
		_comboRenderer = static_cast<MyGUI::ComboBox*>(_gui->FindInLayout(mainLayout, "ComboRenderer"));
		_checkFullscreen = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "CheckFullscreen"));
		_labelFullscreen = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelFullscreen"));
		_checkVSync = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "CheckVSync"));
		_labelVSync = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelVSync"));
		_labelFSAA = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "LabelFSAA"));
		_buttonFSAALeft = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonFSAALeft"));
		_buttonFSAARight = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonFSAARight"));
		_valueFSAA = static_cast<MyGUI::EditBox*>(_gui->FindInLayout(mainLayout, "ValueFSAA"));

		// Set event handlers
		_buttonAccept->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsVideo::ButtonAcceptClicked);
		_comboResoultion->eventComboChangePosition += MyGUI::newDelegate(this, &OptionsVideo::ComboResolutionChanged);
		_comboRenderer->eventComboChangePosition += MyGUI::newDelegate(this, &OptionsVideo::ComboRendererChanged);
		_checkFullscreen->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsVideo::ButtonFullscreenClicked);
		_checkVSync->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsVideo::ButtonVSyncClicked);
		_buttonFSAALeft->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsVideo::ButtonFSAALeftClicked);
		_buttonFSAARight->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsVideo::ButtonFSAARightClicked);

		// Set locale texts
		_buttonAccept->setCaption(GET_TEXT("ButtonApply","Gui.MainMenu.Options.Video"));
		_labelResoultion->setCaption(GET_TEXT("LabelResolution","Gui.MainMenu.Options.Video"));
		_labelRenderer->setCaption(GET_TEXT("LabelRenderer","Gui.MainMenu.Options.Video"));
		_labelFullscreen->setCaption(GET_TEXT("LabelFullscreen","Gui.MainMenu.Options.Video"));
		_labelVSync->setCaption(GET_TEXT("LabelVSync","Gui.MainMenu.Options.Video"));
		_labelFSAA->setCaption(GET_TEXT("LabelFSAA","Gui.MainMenu.Options.Video"));

		// Find settings values from configs
		_newSettings = SystemSettings::GetVideoSettings();

		// Fill controls with data and set current values
		auto videoModes = RenderSystemManager::Instance()->GetAvailableVideoModes();
		int selected = 0;
		for(auto it = videoModes.begin(); it != videoModes.end(); it++)
		{
			_comboResoultion->addItem( *it );
			if( it->compare( _newSettings.VideoMode ) == 0 )
			{
				_comboResoultion->setIndexSelected(selected);
			}
			++selected;
		}
		_comboResoultion->setCaption(_newSettings.VideoMode);

		auto renderers = RenderSystemManager::Instance()->GetAvailableRenderSystems();
		selected = 0;
		for(auto it = renderers.begin(); it != renderers.end(); it++)
		{
			_comboRenderer->addItem( (*it)->getName() );
			if( (*it)->getName().compare( _newSettings.VideoMode ) == 0 )
			{
				_comboRenderer->setIndexSelected(selected);
			}
			++selected;
		}
		_comboRenderer->setCaption(_newSettings.RenderSystem);

		_checkFullscreen->setStateSelected(_newSettings.IsFullscreen);
		_checkVSync->setStateSelected(_newSettings.IsVSync);
		_valueFSAA->setCaption(std::to_string(_newSettings.FSAA));

		auto opts = RenderSystemManager::Instance()->GetOgreRoot()->getRenderSystem()->getConfigOptions();
		auto optIt = opts.find("FSAA");
		auto possibleOptsIt = optIt->second.possibleValues.begin();
		int fsaaIdx = 0;
		while (possibleOptsIt != optIt->second.possibleValues.end())
		{
			int fsaa = std::stoi( *possibleOptsIt );
			_availableFSAAs.push_back(fsaa);
			if( fsaa == _newSettings.FSAA )
				_currentFSAAIdx = fsaaIdx;
			++fsaaIdx;
			++possibleOptsIt;
		}

			_loaded = true;
		//
		ADD_LOG("End Create OptionsVideo");
	}

	void OptionsVideo::Hide()
	{
		if(_gui.IsValid())
			_gui->HideLayout("OptionsVideo.layout");
	}

	void OptionsVideo::Show()
	{
		if(_gui.IsValid())
			_gui->ShowLayout("OptionsVideo.layout");
	}

	void OptionsVideo::SaveSettings()
	{
		_pendingSettingChanges = SystemSettings::GetVideoSettings().IsFullscreen != _newSettings.IsFullscreen ||
			SystemSettings::GetVideoSettings().IsVSync != _newSettings.IsVSync ||
			_newSettings.VideoMode.compare(SystemSettings::GetVideoSettings().VideoMode) != 0;

		_needRestartForChanges = _newSettings.FSAA != SystemSettings::GetVideoSettings().FSAA ||
			_newSettings.VideoMode.compare(SystemSettings::GetVideoSettings().VideoMode) != 0;

		SystemSettings::GetVideoSettings() = _newSettings;
		SystemSettings::GetVideoSettings().SaveSettingsToConfig();
	}

	void OptionsVideo::UndoSettings()
	{
		_newSettings = SystemSettings::GetVideoSettings();
		if(_loaded)
		{
			_checkFullscreen->setStateSelected(SystemSettings::GetVideoSettings().IsFullscreen);
			_checkVSync->setStateSelected(SystemSettings::GetVideoSettings().IsVSync);
			_comboResoultion->setCaption(SystemSettings::GetVideoSettings().VideoMode);
			_comboRenderer->setCaption(SystemSettings::GetVideoSettings().RenderSystem);
			_valueFSAA->setCaption(std::to_string(SystemSettings::GetVideoSettings().FSAA));
			for(unsigned int i = 0; i < _availableFSAAs.size(); ++i)
			{
				if( _availableFSAAs[i] == SystemSettings::GetVideoSettings().FSAA )
				{
					_currentFSAAIdx = (int)i;
					break;
				}
			}
		}
	}

	void OptionsVideo::Unload()
	{
		if(_loaded && _gui.IsValid())
		{
			_loaded = false;
			_gui->UnloadLayout("OptionsVideo.layout");
		}
	}

	ErrorCode OptionsVideo::Update(float ms)
	{
		// If video mode changed change it in rendermanager
		if(_pendingSettingChanges)
		{
			_pendingSettingChanges = false;
			RenderSystemManager::Instance()->SetVideoMode(_newSettings.VideoMode);
			RenderSystemManager::Instance()->SetFullscreen(_newSettings.IsFullscreen);
			RenderSystemManager::Instance()->SetVSync(_newSettings.IsVSync);
		}

		if( _needRestartForChanges )
		{
			// Show info dialog with need restart text
		}

		return ErrorCodes::Success;
	}

	void OptionsVideo::ButtonAcceptClicked(MyGUI::Widget*)
	{
		SaveSettings();
	}

	void OptionsVideo::ButtonFullscreenClicked(MyGUI::Widget*)
	{
		bool oldSelected = _checkFullscreen->getStateSelected();
		_checkFullscreen->setStateSelected( !oldSelected );
		_newSettings.IsFullscreen = !oldSelected;
	}

	void OptionsVideo::ButtonVSyncClicked(MyGUI::Widget*)
	{
		bool oldSelected = _checkVSync->getStateSelected();
		_checkVSync->setStateSelected( !oldSelected );
		_newSettings.IsVSync = !oldSelected;
	}

	void OptionsVideo::ButtonFSAALeftClicked(MyGUI::Widget*)
	{
		if( _currentFSAAIdx > 0 )
		{
			_currentFSAAIdx -= 1;
			int fsaa = _availableFSAAs[_currentFSAAIdx];
			_valueFSAA->setCaption(std::to_string(fsaa));
			_newSettings.FSAA = fsaa;
		}
	}

	void OptionsVideo::ButtonFSAARightClicked(MyGUI::Widget*)
	{
		if( _currentFSAAIdx < (int)_availableFSAAs.size() - 1 )
		{
			_currentFSAAIdx += 1;
			int fsaa = _availableFSAAs[_currentFSAAIdx];
			_valueFSAA->setCaption(std::to_string(fsaa));
			_newSettings.FSAA = fsaa;
		}
	}

	void OptionsVideo::ComboResolutionChanged(MyGUI::ComboBox* sender, size_t index)
	{
		_newSettings.VideoMode = _comboResoultion->getItemNameAt(index);
	}

	void OptionsVideo::ComboRendererChanged(MyGUI::ComboBox* sender, size_t index)
	{
		_newSettings.RenderSystem = _comboRenderer->getItemNameAt(index);
	}
}