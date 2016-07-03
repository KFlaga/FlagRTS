#pragma once

#include "NoticeMessages.h"
#include <Event.h>
#include <GUI.h>

namespace MyGUI
{
	class ImageBox;
	class TextBox;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class NotifyPanel
		{
		private:
			float _messageTime;
			int _maxMessages;
			int _fontHeight;
			int _currMsgCount;
			bool _isVisible;

			MyGUI::ImageBox* _mainPanel;
			Array<MyGUI::TextBox*> _messageBoxes;
			Array<float> _msgTimeouts;

		public:
			NotifyPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~NotifyPanel();

			void Update(float ms);

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }

			void Show();
			void Hide();
			bool IsVisible() const { return _isVisible; }

			void Resize( float ratioHorizontal, float ratioVertical );

			void AddMessage(const NoticeMessage& msg);

		private:
			void RemoveMessage(int num);
		};
	}
}