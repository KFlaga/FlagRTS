#pragma once

#include "GameGuiIUnitSubPanel.h"
#include <GUI.h>
#include <Event.h>

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
		class UnderConstructonPanel : public IUnitSubPanel
		{
		private:
			MyGUI::TextBox* _nameText;
			MyGUI::TextBox* _progressText;

		public:
			UnderConstructonPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent);
			~UnderConstructonPanel();

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