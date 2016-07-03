#include "IMenuScreen.h"

namespace FlagRTS
{
	namespace GameGui
	{
		class GuiElement;
	}

	class QuitConfirmDialog;
	// Main panel screen with buttons for changing to other screen
	class MainScreen : public IMenuScreen
	{
	private:
		MyGUI::ImageBox* _bgPanel; // Backgroung panel - should have bg inmage loaded
		GameGui::GuiElement* _buttonStart;
		GameGui::GuiElement* _buttonOptions;
		GameGui::GuiElement* _buttonQuit;
		bool _loaded; // True if gui is loaded
		bool _pendingQuitDialogClose; // Close quit dialog on next update
		bool _pendingCloseApp; // Close app on next update
		QuitConfirmDialog* _quitDialog;

	public:
		MainScreen();
		~MainScreen();

		// Updates state of menu ( pool pending close/load etc requests which shouldn't be done
		// synchronously )
		ErrorCode Update(float ms);
		// Unloads gui
		void Unload();
		void Hide();

		void Resize(int width, int height);

	private:
		void InitGui();

		void OnButtonStartClicked(GameGui::GuiElement*, int);
		DEFINE_DELEGATE2(ButtonStartClickedDelegate, MainScreen,
			OnButtonStartClicked, GameGui::GuiElement*, int);
		ButtonStartClickedDelegate _onButtonStartClicked;

		void OnButtonOptionsClicked(GameGui::GuiElement*, int);
		DEFINE_DELEGATE2(ButtonOptionsClickedDelegate, MainScreen,
			OnButtonOptionsClicked, GameGui::GuiElement*, int);
		ButtonOptionsClickedDelegate _onButtonOptionsClicked;

		void OnButtonQuitClicked(GameGui::GuiElement*, int);
		DEFINE_DELEGATE2(OnButtonQuitClickedDelegate, MainScreen,
			OnButtonQuitClicked, GameGui::GuiElement*, int);
		OnButtonQuitClickedDelegate _onButtonQuitClicked;
	};
}