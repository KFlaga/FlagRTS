#pragma once

#include <GUI.h>
#include <GUIManager.h>

namespace FlagRTS
{
	// Gui dedicated class
	// Contains GUI with loading screen layout
	// Handles game exit events and is able to update progress bar
	class LoadingScreen
	{
	private:
		RefPtr<GUI> _gui;
		MyGUI::TextBox* _tbLoading;
		MyGUI::TextBox* _tbStatus;

	private:
		void InitGui();

	public:
		LoadingScreen();
		~LoadingScreen();

		void SetLoadingStatus(const std::string& txt);
		void SetLoadingText(const std::string& txt);
		void Show();
		void Hide();
		void Unload();
	};
}