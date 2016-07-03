#pragma once

namespace MyGUI
{
	class ImageBox;
}

namespace FlagRTS
{
	namespace GameGui
	{
		enum FunctionalPanels : int
		{
			UnitInfo = 0,
			SelectedUnits = 1,
			Buildings = 2,

			FP_PanelsCount
		};

		// Panels that are shown on center of bottom gui
		// Changed depending on situation ( units selected / building selection / upgrades etc )
		class IFunctionalPanel
		{
		protected:
			MyGUI::ImageBox* _mainPanel;
			int _panelType;
			bool _isVisible;

		public:
			IFunctionalPanel(int panelType) :
				_panelType(panelType),
				_mainPanel(0),
				_isVisible(false)
			{ }

			~IFunctionalPanel() { }

			virtual void Update(float ms) { }

			virtual void Show() = 0;
			virtual void Hide() = 0;
			bool IsVisible() const { return _isVisible; }

			// Updates size of panel
			// Must be called if parent's size is changed
			// Values are of relative size of parent
			virtual void Resize(float ratioHorizontal, float ratioVertical) = 0;

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }

			int GetPanelType() const { return _panelType; }
		};
	}
}