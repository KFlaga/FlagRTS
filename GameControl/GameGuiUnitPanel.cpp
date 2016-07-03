#include "GameGuiUnitPanel.h"
#include <Unit.h>
#include <MyGUI\MyGUI.h>
#include <GUI.h>

#include "GameGuiUnitStatsSubPanel.h"
#include "GameGuiUnderConstructionPanel.h"
#include "GameGuiUnitTrainingSubplanel.h"
#include "GameGuiModelViewer.h"

namespace FlagRTS
{
	using namespace GameGui;

	UnitPanel::UnitPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		IFunctionalPanel(FunctionalPanels::SelectedUnits),
		_currentUnit(0),
		_currentSubPanel(0),
		_onUnitDespawned(this)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "UnitPanel"));

		IUnitSubPanel* statsPanel = xNew3(UnitStatsSubPanel, gameGui, mainLayout, this);
		IUnitSubPanel* constructionPanel = xNew3(UnderConstructonPanel, gameGui, mainLayout, this);
		IUnitSubPanel* trainingPanel = xNew3(UnitTrainingSubPanel, gameGui, mainLayout, this);

		_subPanels.resize( UnitSubPanels::US_PanelsCount );
		_subPanels[UnitSubPanels::UnitStats] = statsPanel;
		_subPanels[UnitSubPanels::Construction] = constructionPanel;
		_subPanels[UnitSubPanels::Training] = trainingPanel;
		
		MyGUI::Widget* modelPanel = gameGui->FindInLayout(mainLayout, "UP_ModelPanel");
		_model = xNew3(ModelViewer, modelPanel->getWidth(), modelPanel->getHeight(), modelPanel);
		_model->SetVisible(false);
		
		_mainPanel->setVisible(false);
	}

	UnitPanel::~UnitPanel()
	{
		xDelete(_model);
		for(unsigned int i = 0; i < _subPanels.size(); ++i)
		{
			xDeleteSafe(_subPanels[i]);
		}
	}

	void UnitPanel::Update(float ms)
	{
		if(_currentSubPanel != 0 && _currentSubPanel->IsVisible())
		{
			// _ASSERT(_currentUnit != 0);

			_currentSubPanel->Update(ms);
			if(_currentUnit != 0)
				_model->Update(ms);
		}
	}

	void UnitPanel::ChangeSubPanel(int type)
	{
		IUnitSubPanel* subpanel = _subPanels[type];
		if(subpanel == _currentSubPanel)
		{
			_currentSubPanel->SetCurrentUnit(_currentUnit);
			return;
		}

		if(_currentSubPanel != 0)
		{
			_currentSubPanel->Hide();
			_currentSubPanel->SetCurrentUnit(0);
		}
		_currentSubPanel = subpanel;
		_currentSubPanel->SetCurrentUnit(_currentUnit);
		_currentSubPanel->Show();
	}

	void UnitPanel::Show()
	{
		_mainPanel->setVisible(true);
		if(_currentSubPanel != 0)
		{
			_currentSubPanel->SetCurrentUnit(_currentUnit);
			_currentSubPanel->Show();
		}
		_model->SetVisible(true);
		_isVisible = true;
	}

	void UnitPanel::Hide()
	{
		_mainPanel->setVisible(false);
		if(_currentSubPanel != 0)
		{
			_currentSubPanel->Hide();
			_currentSubPanel->SetCurrentUnit(0);
		}
		_model->SetVisible(false);
		_isVisible = false;
	}

	void UnitPanel::SetCurrentUnit(Unit* unit)
	{
		if(_currentUnit != 0)
		{
			_currentUnit->Despawned() -= &_onUnitDespawned;
		}

		_currentUnit = unit;
		if(_currentUnit != 0)
		{
			_currentUnit->Despawned() += &_onUnitDespawned;

			auto unitDef = unit->GetUnitDefinition();
			if( unitDef->GetPortraitDefinition() != 0 )
				_model->SetViewedObject(unitDef->GetPortraitDefinition());
			else
				_model->SetViewedObject(unitDef);

			_model->GetViewedObject()->SetOwner(unit->GetOwner());
			
			if(unitDef->GetUseDefaultPortraitPosition() == false)
			{
				_model->SetObjectPosition(unitDef->GetPortraitPosition());
			}
			_model->SetObjectOrientation(unitDef->GetPortraitOrientation());
			
		}
		else
			_model->SetViewedObject(0);
	}

	void UnitPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _mainPanel->getCoord();
		_mainPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		for(unsigned int i = 0; i < _subPanels.size(); ++i)
		{
			_subPanels[i]->Resize(ratioHorizontal, ratioVertical);
		}
		_model->Resize(ratioHorizontal, ratioVertical);
	}

	void UnitPanel::OnUnitDespawned(SceneObject*)
	{
		_currentUnit = 0;
		Hide();
	}
}