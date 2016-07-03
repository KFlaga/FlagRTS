#include "MainScreen.h"
#include <GUIManager.h>
#include <LogManager.h>
#include <LocalizationManager.h>
#include <ErrorCodes.h>
#include <LogManager.h>
#include "QuitConfirmDialog.h"
#include <GuiElement.h>

namespace FlagRTS
{
	MainScreen::MainScreen() :
		IMenuScreen(xNew1(GUI,"MainScreen"), "MainScreen"),
		_quitDialog(0),
		_onButtonStartClicked(this),
		_onButtonOptionsClicked(this),
		_onButtonQuitClicked(this)
	{
		ADD_LOG("Begin Creating MainScreen");
		InitGui();
		_loaded = true;
		_pendingCloseApp = false;
		_pendingQuitDialogClose = false;
		ADD_LOG("End Creating MainScreen");
	}

	MainScreen::~MainScreen()
	{
		Unload();
	}

	ErrorCode MainScreen::Update(float ms)
	{
		if(_quitDialog != 0 && _quitDialog->IsClosePending())
		{
			_quitDialog->Unload();
			_pendingCloseApp = _quitDialog->IsAccepted();
			xDelete(_quitDialog);
			_quitDialog = 0;
		}

		if(_pendingCloseApp)
		{
			_pendingCloseApp = false;
			return ErrorCodes::Exit;
		}

		return ErrorCodes::Success;
	}

	void MainScreen::InitGui()
	{
		// InitGui
		if( _gui->LoadLayout("MainScreen.layout", 0, true) != ErrorCodes::Success)
		{
			ADD_LOG("Couldn't load layout : MainScreen.layout");
		}

		// Assign widgets
		RefPtr<Layout> mainLayout = _gui->GetLayout("MainScreen.layout");
		_bgPanel = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(mainLayout, "MainPanel"));
		_buttonStart = xNew3(GameGui::GuiElement,_gui, mainLayout, "ButtonStart");
		_buttonOptions = xNew3(GameGui::GuiElement,_gui, mainLayout, "ButtonOptions");
		_buttonQuit = xNew3(GameGui::GuiElement,_gui, mainLayout, "ButtonQuit");

		// Set button texts
		_buttonStart->GetMyGUIWidget<MyGUI::Button>()->setCaption(GET_TEXT("ButtonStart","Gui.MainMenu.MainScreen"));
		_buttonOptions->GetMyGUIWidget<MyGUI::Button>()->setCaption(GET_TEXT("ButtonOptions","Gui.MainMenu.MainScreen"));
		_buttonQuit->GetMyGUIWidget<MyGUI::Button>()->setCaption(GET_TEXT("ButtonQuit","Gui.MainMenu.MainScreen"));

		// Set event handlers for buttons:
		_buttonStart->GetMouseClickedEvent() += &_onButtonStartClicked;
		_buttonOptions->GetMouseClickedEvent() += &_onButtonOptionsClicked;
		_buttonQuit->GetMouseClickedEvent() += &_onButtonQuitClicked;

		_gui->AddGuiElement(_buttonStart);
		_gui->AddGuiElement(_buttonOptions);
		_gui->AddGuiElement(_buttonQuit);

		GUIManager::Instance()->RegisterGUI("MainScreen", _gui); 
	}

	void MainScreen::Hide()
	{
		_pendingCloseApp = false;
		_pendingQuitDialogClose = false;
		IMenuScreen::Hide();
	}

	void MainScreen::OnButtonStartClicked(GameGui::GuiElement* gButton, int mButton)
	{
		if( mButton == MouseButtons::Left )
			_changeScreenRequested.Fire("CreateGameScreen");
	}

	void MainScreen::OnButtonOptionsClicked(GameGui::GuiElement* gButton, int mButton)
	{
		if( mButton == MouseButtons::Left )
			_changeScreenRequested.Fire("Options");
	}

	void MainScreen::OnButtonQuitClicked(GameGui::GuiElement* gButton, int mButton)
	{
		if( mButton == MouseButtons::Left )
		{
			// Create modal quit dialog
			// Show dialog() -> function returns when closed if possible -> nope
			// it would block, so have a callback to QuitConfirmed
			// Check result -> if true -> exit app
			// if false -> just return
			_quitDialog = xNew1(QuitConfirmDialog,_gui);
			_quitDialog->Show();
		}
	}

	void MainScreen::Unload()
	{
		if(_gui.IsValid() && _loaded)
		{
			ADD_LOG("Unloading MainScreen");
			GUIManager::Instance()->UnregisterGUI("MainScreen");
			xDelete(_buttonStart);
			xDelete(_buttonOptions);
			xDelete(_buttonQuit);
			_gui->UnloadAllLayouts();
			_loaded = false;
		}
	}

	void MainScreen::Resize(int width, int height)
	{

	}
}