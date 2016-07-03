#include "GameGuiUnitStatsSubPanel.h"
#include <Unit.h>
#include <MyGUI\MyGUI.h>
#include "GameGuiUnitPanel.h"

namespace FlagRTS
{
	using namespace GameGui;

	UnitStatsSubPanel::UnitStatsSubPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent) :
		IUnitSubPanel(UnitSubPanels::UnitStats, parent)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "UnitStatsSubPanel"));

		// Create info textboxes
		_nameText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UStatsSP_Text_Name"));
		_healthText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UStatsSP_Text_Health"));

		_mainPanel->setVisible(false);
	}

	UnitStatsSubPanel::~UnitStatsSubPanel()
	{
	}

	void UnitStatsSubPanel::Update(float ms)
	{
		if(_currentUnit == 0)
			return;

		// Check if unit started training -> if yes switch to training panel
		if(_currentUnit->CanTrainUnits() && _currentUnit->IsTrainingUnit() )
		{
			_parent->ChangeSubPanel(GameGui::UnitSubPanels::Training);
			return;
		}

		_healthText->setCaption(string("Health: ") + 
			std::to_string(_currentUnit->GetHitPoints()) + " / " +
			std::to_string(_currentUnit->GetMaxHitPoints()));

	}

	void UnitStatsSubPanel::Show()
	{
		_mainPanel->setVisible(true);
		_isVisible = true;
	}

	void UnitStatsSubPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_isVisible = false;
	}

	void UnitStatsSubPanel::SetCurrentUnit(Unit* unit)
	{
		_currentUnit = unit;
		if(_currentUnit != 0)
		{
			_nameText->setCaption(unit->GetGuiName());
			_healthText->setCaption(string("Health: 0 / 0"));
		}
	}

	void UnitStatsSubPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _mainPanel->getCoord();
		_mainPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		const MyGUI::IntCoord& nameCoord = _nameText->getCoord();
		_nameText->setCoord(
			(int)((float)nameCoord.left * ratioHorizontal), (int)((float)nameCoord.top * ratioVertical), 
			(int)((float)nameCoord.width * ratioHorizontal), (int)((float)nameCoord.height * ratioVertical));

		const MyGUI::IntCoord& healthCoord = _healthText->getCoord();
		_healthText->setCoord(
			(int)((float)healthCoord.left * ratioHorizontal), (int)((float)healthCoord.top * ratioVertical), 
			(int)((float)healthCoord.width * ratioHorizontal), (int)((float)healthCoord.height * ratioVertical));
	}
}