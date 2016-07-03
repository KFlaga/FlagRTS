#pragma once

namespace MyGUI
{
	class ListBox;
	class Widget;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class PerformancePanel
		{
			float _worstFps;
			float _lastFps;
			MyGUI::ListBox* _perfBox;

		public:
			PerformancePanel(MyGUI::Widget* parent = 0);
			~PerformancePanel();
			
			void Update(float ms);

			MyGUI::ListBox* GetMainWidget() { return _perfBox; }

			void SetVisible(bool value);
			bool IsVisible() const;
		};
	}
}