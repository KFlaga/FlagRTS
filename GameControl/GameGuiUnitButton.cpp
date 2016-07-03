#include "GameGuiUnitButton.h"
#include <Unit.h>

#include <MyGUI\MyGUI.h>

namespace FlagRTS
{
	using namespace GameGui;

	UnitButton::UnitButton(MyGUI::Widget* parent) :
		_unit(0),
		_isHighlighted(false)
	{
		_frame = parent->createWidgetReal<MyGUI::Widget>("BorderRed",
				0.0f, 0.0f, 0.01f, 0.01f, MyGUI::Align::Center);
		_button = parent->createWidgetReal<MyGUI::Button>("ButtonImage",
				0.0f, 0.0f, 0.01f, 0.01f, MyGUI::Align::Center);
	}

	UnitButton::~UnitButton()
	{

	}

	void UnitButton::Show()
	{
		_button->setVisible(true);
		_frame->setVisible(_isHighlighted);
	}

	void UnitButton::Hide()
	{
		_button->setVisible(false);
		_frame->setVisible(false);
	}

	void UnitButton::EnableHighlight()
	{
		_isHighlighted = true;
		_frame->setVisible(true);
	}

	void UnitButton::DisableHighlight()
	{
		_isHighlighted = false;
		_frame->setVisible(false);
	}

	void UnitButton::Resize(int left, int top, int width, int height, int frameWidth, int frameHeight)
	{
		_button->setCoord(left, top, width, height);
		_frame->setCoord(left-frameWidth, top-frameHeight, width+2*frameWidth, height+2*frameHeight);
	}

	void UnitButton::SetUnit(Unit* unit)
	{
		_unit = unit;
		if(unit != 0)
		{
			_button->setVisible(true);
			_button->setImageResource( unit->GetUnitDefinition()->GetIcon() );
			_button->setModeImage( true );
		}
		else
		{
			_button->setVisible(false);
		}
	}
}