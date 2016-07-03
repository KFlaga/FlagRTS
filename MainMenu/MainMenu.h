#pragma once

#include <GUI.h>
#include "IMenuScreen.h"

namespace FlagRTS
{
	struct NewGameSettings;

	// Main menu handles showing and changes of proper guis
	class MainMenu
	{
	private:
		Array<IMenuScreen*> _menuScreens;
		IMenuScreen* _currentScreen;
		Event<NewGameSettings*> _newGameRequested;
		string _changeScreenName;
		bool _startGamePending;
		bool _changeScreenPending;

	public:
		MainMenu();
		~MainMenu();

		// Updates state of menu ( handles pending show/load requests etc )
		ErrorCode Update(float ms);
		// Unloads all menu guis
		void Unload();
		// Show current menu gui
		void Show();
		// Hides current menu gui
		void Hide();
		// Changes current menu gui
		void SetCurrentScreen(const string& name);

		// Check from parent level every frame, if true
		// close MM and begin game loading
		bool IsStartGamePending() const { return _startGamePending; }

		Event<NewGameSettings*>& GetNewGameRequestedEvent() { return _newGameRequested; }
		
	private:
		// Returns to main menu from options screen ( or load etc )
		void ReturnToMenu();

		void ChangeScreenRequested(string screenName);
		DEFINE_DELEGATE1(ChangeScreenRequestedDelegate, MainMenu, 
			ChangeScreenRequested, string);
		ChangeScreenRequestedDelegate _onChangeScreenRequested;

		void NewGameRequested(NewGameSettings* gameSettings);
		DEFINE_DELEGATE1(NewGameRequestedDelegate, MainMenu, 
			NewGameRequested, NewGameSettings*);
		NewGameRequestedDelegate _onNewGameRequested;
		
		void ResizeGui(std::pair<int,int> size);
		typedef std::pair<int,int> SizePair;
		DEFINE_DELEGATE1(ResizeGuiDelegate, MainMenu, 
			ResizeGui, SizePair);
		ResizeGuiDelegate _resizeGui;
	};
}