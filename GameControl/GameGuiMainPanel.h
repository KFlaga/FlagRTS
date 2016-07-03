#pragma once

#include <GUI.h>
#include <ArrayMap.h>

namespace MyGUI
{
	class Widget;
	class Button;
	class ImageBox;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class MenuWindow;
		class CommandButtonsPanel;
		class MinimapPanel;
		class PerformancePanel;
		class ResourcesPanel;
		class IFunctionalPanel;
		class FunctionalButtonsPanel;
		class NotifyPanel;
		class ConsolePanel;

		// Invisible container for all ingame gui elements and root widget of GUI
		// All elements of game gui are created in code, except popup game-menu, unlike
		// other guis, becouse they size and content will be determined in run-time
		class MainPanel
		{
		private:
			RefPtr<GUI> _gui;
			MyGUI::Widget* _mainPanel;
			MenuWindow* _menuWindow;
			PerformancePanel* _perfPanel;
			MyGUI::ImageBox* _bottomPanel;
			CommandButtonsPanel* _buttonsPanel;
			MinimapPanel* _minimapPanel;
			ResourcesPanel* _resourcesPanel;
			FunctionalButtonsPanel* _funcButtonsPanel;
			IFunctionalPanel* _currentFuncPanel;
			NotifyPanel* _notifyPanel;
			ConsolePanel* _consolePanel;
			
			Array<IFunctionalPanel*> _funcPanels;

		public:
			MainPanel();
			~MainPanel();

			void Update(float ms);
			
			void Show();
			void Hide();
			
			void Resize(int screenWidth, int screenHeight);

			MyGUI::Widget* GetMainPanel() { return _mainPanel; }
			MyGUI::ImageBox* GetBottomPanel() { return _bottomPanel; }
			CommandButtonsPanel* GetCommandButtonsPanel() { return _buttonsPanel; }
			PerformancePanel* GetPerformancePanel() { return _perfPanel; }
			ResourcesPanel* GetResourcesPanel() { return _resourcesPanel; }
			FunctionalButtonsPanel* GetFunctionalButtonsPanel() { return _funcButtonsPanel; }
			NotifyPanel* GetNotifyPanel() { return _notifyPanel; }
			ConsolePanel* GetConsolePanel() { return _consolePanel; }
			MinimapPanel* GetMinimapPanel() { return _minimapPanel; }

			template<typename PanelType = IFunctionalPanel>
			PanelType* GetFunctionalPanel(int type) 
			{ 
				return static_cast<PanelType*>(_funcPanels[type]); 
			}

			void ResetFunctionalPanel();
			IFunctionalPanel* GetCurrentFunctionalPanel() { return _currentFuncPanel; }
			void ChangeFunctionalPanel(int type);

		private:
			// Allocates and creates all gui elements
			void CreateGui();
			// Creates Ogre textures
			void LoadGuiImages();
		};
	}
}