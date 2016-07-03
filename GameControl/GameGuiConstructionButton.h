#pragma once

namespace MyGUI
{
	class Widget;
	class Button;
}

namespace FlagRTS
{
	struct TechBuilding;

	namespace GameGui
	{
		// Small buttons that are shown on panels that are used for
		// units construction ( buildings / unit training )
		class ConstructionButton
		{
		private:
			TechBuilding* _building;
			MyGUI::Button* _button;
			bool _isEnabled;

		public:
			ConstructionButton(MyGUI::Widget* parent);
			~ConstructionButton();

			void Show();
			void Hide();

			void SetConstructionEnabled(bool value);

			bool IsEnabled() const { return _isEnabled; }

			// Updates size of button
			// Must be called if parent's size is changed
			// Size is absoulute, position is relative to parent
			void Resize(int left, int top, int width, int height);

			void SetBuilding(TechBuilding* unit);
			TechBuilding* GetBuilding() const { return _building; }

			MyGUI::Button* GetButton() { return _button; }
		};
	}
}