#include "GameGuiUnderConstructionPanel.h"
#include <Unit.h>
#include <MyGUI\MyGUI.h>
#include "GameGuiUnitPanel.h"

namespace FlagRTS
{
	using namespace GameGui;

	UnderConstructonPanel::UnderConstructonPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent) :
		IUnitSubPanel(UnitSubPanels::Construction, parent)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "UnitConstructionSubPanel"));

		// Create info textboxes
		_nameText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UConstrusctionSP_Text_Name"));
		_progressText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UConstrusctionSP_Text_Progress"));
		
		_mainPanel->setVisible(false);
	}

	UnderConstructonPanel::~UnderConstructonPanel()
	{

	}

	void UnderConstructonPanel::Update(float ms)
	{
		if(_currentUnit == 0)
			return;

		if(!_currentUnit->IsUnderConstruction())
		{
			_parent->ChangeSubPanel(UnitSubPanels::UnitStats);
			return;
		}

		int progress = (int)(_currentUnit->GetConstructionProgress()*100.f);
		_progressText->setCaption(
			string("Under construction: ") + std::to_string(progress) + "%");
	}

	void UnderConstructonPanel::Show()
	{
		_mainPanel->setVisible(true);
		_isVisible = true;
	}

	void UnderConstructonPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_isVisible = false;
	}

	void UnderConstructonPanel::SetCurrentUnit(Unit* unit)
	{
		_currentUnit = unit;
		if(_currentUnit != 0)
		{
			_nameText->setCaption(unit->GetGuiName());
			_progressText->setCaption(
				string("Under construction: 0%"));
		}
	}

	void UnderConstructonPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _mainPanel->getCoord();
		_mainPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		const MyGUI::IntCoord& nameCoord = _nameText->getCoord();
		_nameText->setCoord(
			(int)((float)nameCoord.left * ratioHorizontal), (int)((float)nameCoord.top * ratioVertical), 
			(int)((float)nameCoord.width * ratioHorizontal), (int)((float)nameCoord.height * ratioVertical));

		const MyGUI::IntCoord& progCoord = _progressText->getCoord();
		_progressText->setCoord(
			(int)((float)progCoord.left * ratioHorizontal), (int)((float)progCoord.top * ratioVertical), 
			(int)((float)progCoord.width * ratioHorizontal), (int)((float)progCoord.height * ratioVertical));
	}
}