#pragma once

#include "GameGuiIUnitSubPanel.h"
#include <GUI.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class TextBox;
}

namespace FlagRTS
{
	class Unit;

	namespace GameGui
	{
		class UnitStatsSubPanel : public IUnitSubPanel
		{
		private:
			MyGUI::TextBox* _nameText;
			MyGUI::TextBox* _healthText;

		public:
			UnitStatsSubPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent);
			~UnitStatsSubPanel();

			// Updates current info on unit, as it could change
			void Update(float ms);

			void Show();
			void Hide();

			// Changes unit whose info is currently dispalyed
			void SetCurrentUnit(Unit* unit);

			void Resize(float ratioHorizontal, float ratioVertical);
		};
	}
}