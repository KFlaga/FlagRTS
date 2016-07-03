#pragma once

namespace MyGUI
{
	class Widget;
	class Button;
}

namespace FlagRTS
{
	class Unit;

	namespace GameGui
	{
		// Small buttons that are shown on selection panel
		// Button contains info about unit it represents
		class UnitButton
		{
		private:
			Unit* _unit;
			MyGUI::Button* _button;
			MyGUI::Widget* _frame;
			bool _isHighlighted;

		public:
			UnitButton(MyGUI::Widget* parent);
			~UnitButton();

			void Show();
			void Hide();

			// Highlight's button frame ( when unit is in active subgroup )
			void EnableHighlight();
			// Disbale highlighting of button frame ( when unit is in active subgroup )
			void DisableHighlight();

			bool IsHighlighted() const { return _isHighlighted; }

			// Updates size of button
			// Must be called if parent's size is changed
			// Size is absoulute, position is relative to parent
			void Resize(int left, int top, int width, int height, int frameWidth, int frameHeight);

			void SetUnit(Unit* unit);
			Unit* GetUnit() const { return _unit; }

			MyGUI::Button* GetButton() { return _button; }
		};
	}
}