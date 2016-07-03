#include "GameGuiConstructionButton.h"
#include <Technology.h>
#include <UnitDefinition.h>

#include <MyGUI\MyGUI.h>

namespace FlagRTS
{
	using namespace GameGui;

	ConstructionButton::ConstructionButton(MyGUI::Widget* parent) :
		_building(0),
		_isEnabled(true)
	{
		_button = parent->createWidgetReal<MyGUI::Button>("ButtonImage",
				0.0f, 0.0f, 0.01f, 0.01f, MyGUI::Align::Center);
	}

	ConstructionButton::~ConstructionButton()
	{

	}

	void ConstructionButton::Show()
	{
		_button->setVisible(true);
	}

	void ConstructionButton::Hide()
	{
		_button->setVisible(false);
	}

	void ConstructionButton::SetConstructionEnabled(bool value)
	{
		_isEnabled = value;
		_button->setEnabled(value);
	}

	void ConstructionButton::Resize(int left, int top, int width, int height)
	{
		_button->setCoord(left, top, width, height);
	}

	void ConstructionButton::SetBuilding(TechBuilding* build)
	{
		_building = build;
		if(build != 0)
		{
			_button->setVisible(true);
			_button->setImageResource( build->Building->GetIcon() );
			_button->setModeImage( true );
		}
		else
		{
			_button->setVisible(false);
		}
	}
}