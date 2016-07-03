#pragma once

#include <Array.h>
#include <GUI.h>
#include <Event.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class TextBox;
	struct ToolTipInfo;
}

namespace FlagRTS
{
	class Resources;

	namespace GameGui
	{
		class ResourcesPanel
		{
		private:
			MyGUI::ImageBox* _mainPanel;
			Array<MyGUI::ImageBox*> _resourceFieldsIcons;
			Array<MyGUI::TextBox*> _resourceFieldsValues;
			MyGUI::ImageBox* _tooltipResource;
			MyGUI::TextBox* _tooltipResourceText;
			Resources* _resources;
			float _updateTickLength;
			float _tickTimer;

			bool _isVisible;

		public:
			ResourcesPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~ResourcesPanel();

			// Updates current info on resources
			void Update(float ms);

			void Show();
			void Hide();

			bool IsVisible() const { return _isVisible; }


			void Resize(float ratioHorizontal, float ratioVertical);

			MyGUI::ImageBox* GetPanel() { return _mainPanel ; }
			const Array<MyGUI::ImageBox*>& GetResourceIcons() { return _resourceFieldsIcons; }
			const Array<MyGUI::TextBox*>& GetResourceValuePanels() { return _resourceFieldsValues; }

		private:
			void ShowTooltip(MyGUI::Widget* sender, const MyGUI::ToolTipInfo& info);
		};
	}
}