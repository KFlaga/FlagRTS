#include "GameGuiNotifyPanel.h"

#include <MyGUI\MyGUI.h>
#include <ConfigurationManager.h>

#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	using namespace GameGui;

	Vector3 ncolors[] =
	{
		Vector3(0.9f, 1.f, 0.2f),
		Vector3(0.9f, 1.f, 0.2f),
		Vector3(1.f, 0.1f, 0.1f),
		Vector3(1.f, 0.1f, 0.1f)
	};

	namespace Notices
	{
		const Vector3* NoticeTypeColors = ncolors;
	}

	inline int GetMsgNum(const int nr, const int maxMsg) { return maxMsg - nr - 1; }

	NotifyPanel::NotifyPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		_maxMessages(5),
		_currMsgCount(0),
		_messageTime(10000.f)
	{
		auto& conf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		_maxMessages = std::stoi(conf->FindOptionValue("MaxMsg", "NotifyPanel"));
		_messageTime = (float)std::stoi(conf->FindOptionValue("MsgTime", "NotifyPanel"));
		_fontHeight = std::stoi(conf->FindOptionValue("TextHeight", "NotifyPanel"));

		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "NotifyPanel"));
		_msgTimeouts.resize(_maxMessages, 0.f);
		_messageBoxes.resize(_maxMessages, 0);

		_mainPanel->setVisible(false);
		_isVisible = false;
		_currMsgCount = 0;

		for(int i = 0; i < _maxMessages; ++i)
		{
			_messageBoxes[i] = _mainPanel->createWidgetReal<MyGUI::TextBox>("TextBox",
				0.f, (float)i/(float)_maxMessages, 1.f, 1.f/(float)_maxMessages, 
				MyGUI::Align::Default, "NoticeText_" + std::to_string(i));

			_messageBoxes[i]->setTextAlign(MyGUI::Align::Left);
			_messageBoxes[i]->setFontHeight(_fontHeight);
		}
	}

	NotifyPanel::~NotifyPanel()
	{
		
	}

	void NotifyPanel::Update(float ms)
	{
		if(IsVisible())
		{
			for(int i = 0; i < _currMsgCount;)
			{
				_msgTimeouts[i] -= ms;
				if(_msgTimeouts[i] <= 0.f)
				{
					RemoveMessage(i);
					continue;
				}
				++i;
			}
		}
	}
	
	void NotifyPanel::AddMessage(const NoticeMessage& msg)
	{
		if(_currMsgCount >= _maxMessages)
			RemoveMessage(0);

		// Just put message on back
		_msgTimeouts[_currMsgCount] = _messageTime;
		_messageBoxes[ GetMsgNum(_currMsgCount, _maxMessages) ]->setCaption(msg.Message);
		auto& color = Notices::NoticeTypeColors[msg.Type];
		_messageBoxes[ GetMsgNum(_currMsgCount, _maxMessages) ]->setTextColour( MyGUI::Colour(
			 color.x, color.y, color.z) );
		++_currMsgCount;
	}

	void NotifyPanel::Show()
	{
		_mainPanel->setVisible(true);
		_isVisible = true;
	}

	void NotifyPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_isVisible = false;
	}

	void NotifyPanel::RemoveMessage(int nr)
	{
		_ASSERT(nr < _maxMessages && nr < _currMsgCount && nr >= 0);
		for(int i = nr; i < _currMsgCount - 1; ++i)
		{
			_messageBoxes[ GetMsgNum(i, _maxMessages) ]->setCaption(
				_messageBoxes[ GetMsgNum(i+1, _maxMessages) ]->getCaption());
			_messageBoxes[ GetMsgNum(i, _maxMessages) ]->setTextColour(
				_messageBoxes[ GetMsgNum(i+1, _maxMessages) ]->getTextColour());
			_msgTimeouts[i] = _msgTimeouts[i+1];
		}
		_messageBoxes[ GetMsgNum(_currMsgCount-1, _maxMessages) ]->setCaption("");

		--_currMsgCount;
	}

	void NotifyPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _mainPanel->getCoord();
		_mainPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		for(int i = 0; i < _maxMessages; ++i)
		{
			const MyGUI::IntCoord& textCoords = _messageBoxes[i]->getCoord();
			_messageBoxes[i]->setCoord(
			(int)((float)textCoords.left * ratioHorizontal), (int)((float)textCoords.top * ratioVertical), 
			(int)((float)textCoords.width * ratioHorizontal), (int)((float)textCoords.height * ratioVertical));
		}
	}
}