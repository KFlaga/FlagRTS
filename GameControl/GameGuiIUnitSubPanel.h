#pragma once

namespace MyGUI
{
	class ImageBox;
}

namespace FlagRTS
{
	class Unit;

	namespace GameGui
	{
		class UnitPanel;
		enum UnitSubPanels : int
		{
			UnitStats = 0,
			Construction = 1,
			Training = 2,

			US_PanelsCount
		};

		// Panels that main part of UnitPanel - depending on unit type/state
		// it shows different kind of info
		class IUnitSubPanel
		{
		protected:
			UnitPanel* _parent;
			MyGUI::ImageBox* _mainPanel;
			Unit* _currentUnit;
			int _panelType;
			bool _isVisible;

		public:
			IUnitSubPanel(int panelType, UnitPanel* parent) :
				_panelType(panelType),
				_parent(parent),
				_mainPanel(0),
				_isVisible(false),
				_currentUnit(0)
			{ }

			~IUnitSubPanel() { }

			virtual void Update(float ms) { }

			virtual void Show() = 0;
			virtual void Hide() = 0;
			bool IsVisible() const { return _isVisible; }

			virtual void SetCurrentUnit(Unit* unit)
			{
				_currentUnit = unit;
			}

			// Updates size of panel
			// Must be called if parent's size is changed
			// Values are of relative size of parent
			virtual void Resize(float ratioHorizontal, float ratioVertical) = 0;

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }

			int GetPanelType() const { return _panelType; }
			UnitPanel* GetParentPanel() { return _parent; }

		};
	}
}