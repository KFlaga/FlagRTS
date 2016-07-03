#pragma once

#include <Event.h>
#include <GUI.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class Button;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class GuiElement;
		class FunctionalButtonsPanel
		{
		private:
			MyGUI::ImageBox* _mainPanel;
			GuiElement* _buttonSelection;
			GuiElement* _buttonBuildings;

			Event<int> _functionalPanelSwitchRequested;

		public:
			FunctionalButtonsPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~FunctionalButtonsPanel();

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }

			void Show();
			void Hide();

			void Resize( float ratioHorizontal, float ratioVertical );

			Event<int>& FunctionalPanelSwitchRequested() { return _functionalPanelSwitchRequested; }

		private:
			void OnButtonSelectionClicked(GuiElement*, int);
			DEFINE_DELEGATE2(ButtonSelectionClickedDelegate, FunctionalButtonsPanel,
				OnButtonSelectionClicked, GuiElement*, int);
			ButtonSelectionClickedDelegate _onButtonSelectionClicked;

			void OnButtonBuildingsClicked(GuiElement*, int);
			DEFINE_DELEGATE2(ButtonBuildingsClickedDelegate, FunctionalButtonsPanel,
				OnButtonBuildingsClicked, GuiElement*, int);
			ButtonBuildingsClickedDelegate _onButtonBuildingsClicked;
		};
	}
}