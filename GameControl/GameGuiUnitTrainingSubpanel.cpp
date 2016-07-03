#include "GameGuiUnitTrainingSubplanel.h"
#include <Unit.h>
#include <MyGUI\MyGUI.h>
#include "GameGuiUnitPanel.h"

#include <TrainingQueue.h>

namespace FlagRTS
{
	using namespace GameGui;

	UnitTrainingSubPanel::UnitTrainingSubPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent) :
		IUnitSubPanel(UnitSubPanels::Training, parent),
		_onUnitTrained(this),
		_onUnitRemovedFromQueue(this),
		_onUnitQueued(this),
		_trainAbortPending(false),
		_removeFromQueuePending(false),
		_removePosition(0)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "UnitTrainingSubPanel"));

		_nameText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UTrainingSSP_Text_Name"));
		_progressText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(mainLayout, "UTrainingSP_Text_Progress"));
		_currentUnitButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(mainLayout, "UTrainingSP_CurrentUnit"));
		_currentUnitButton->eventMouseButtonClick += 
			MyGUI::newDelegate(this, &UnitTrainingSubPanel::OnCurrentUnitButtonClicked);

		for(unsigned int i = 0; i < 5; ++i)
		{
			MyGUI::Button* queueButton = static_cast<MyGUI::Button*>(gameGui->FindInLayout(
				mainLayout, "UTrainingSP_Queue" + std::to_string(i)));
			queueButton->eventMouseButtonClick += 
				MyGUI::newDelegate(this, &UnitTrainingSubPanel::OnQueuedUnitButtonClicked);
			queueButton->setUserData(MyGUI::Any(i));
			_queueButtons.push_back(queueButton);
		}

		_mainPanel->setVisible(false);
	}

	UnitTrainingSubPanel::~UnitTrainingSubPanel()
	{

	}

	void UnitTrainingSubPanel::Update(float ms)
	{
		if(_currentUnit == 0)
			return;

		// Check if unit stopped training -> if yes switch to stats panel
		if(_currentUnit->IsTrainingUnit() == false)
		{
			_parent->ChangeSubPanel(GameGui::UnitSubPanels::UnitStats);
			return;
		}

		if( _trainAbortPending )
		{
			_currentUnit->GetTrainingQueue()->AbortTraining();
			_trainAbortPending = false;
		}
		else if( _removeFromQueuePending )
		{
			_currentUnit->GetTrainingQueue()->RemoveAt( _removePosition );
			_removeFromQueuePending = false;
		}
		else
		{
			int progress = (int)(_currentUnit->GetTrainingQueue()->
				GetCurrentlyTrainedUnit()->GetConstructionProgress()*100.f);
			if( progress >= 100 )
			{
				_progressText->setCaption("Progress: waiting for spawn area to be cleared");
			}
			else
			{
				_progressText->setCaption("Progress: "  + std::to_string(progress) + "%");
			}
		}
	}

	void UnitTrainingSubPanel::Show()
	{
		_mainPanel->setVisible(true);
		_isVisible = true;
	}

	void UnitTrainingSubPanel::Hide()
	{
		_mainPanel->setVisible(false);
		_isVisible = false;
	}

	void UnitTrainingSubPanel::SetCurrentUnit(Unit* unit)
	{
		if( _currentUnit != 0 )
		{
			// remove events handlers
			_currentUnit->GetTrainingQueue()->UnitAddedToQueue() -= &_onUnitQueued;
			_currentUnit->GetTrainingQueue()->UnitTrained() -= &_onUnitTrained;
			_currentUnit->GetTrainingQueue()->QueuedUnitRemoved() -= &_onUnitRemovedFromQueue;
		}
		_currentUnit = unit;
		_trainAbortPending = false;
		_removeFromQueuePending = false;
		if( _currentUnit != 0 )
		{
			_currentUnit->GetTrainingQueue()->UnitAddedToQueue() += &_onUnitQueued;
			_currentUnit->GetTrainingQueue()->UnitTrained() += &_onUnitTrained;
			_currentUnit->GetTrainingQueue()->QueuedUnitRemoved() += &_onUnitRemovedFromQueue;
			SetTrainingButtons();
		}
	}

	void UnitTrainingSubPanel::OnCurrentUnitButtonClicked(MyGUI::Widget* button)
	{
		_trainAbortPending = true;
	}

	void UnitTrainingSubPanel::OnQueuedUnitButtonClicked(MyGUI::Widget* button)
	{
		_removeFromQueuePending = true;
		_removePosition = *button->getUserData<unsigned int>();
	}

	void UnitTrainingSubPanel::SetTrainingButtons()
	{
		// Hide queue buttons
		for(unsigned int i = 0; i < _queueButtons.size(); ++i)
		{
			_queueButtons[i]->setVisible(false);
		}

		// Set currently trained unit button
		auto trainQueue = _currentUnit->GetTrainingQueue();
		Unit* trainedUnit = trainQueue->GetCurrentlyTrainedUnit();
		if( trainedUnit == 0 )
		{
			_currentUnitButton->setVisible(false);
			return;
		}

		_currentUnitButton->setVisible(true);
		_currentUnitButton->setImageResource( trainedUnit->GetUnitDefinition()->GetIcon() );
		_nameText->setCaption("Training: " + trainedUnit->GetGuiName());
		_progressText->setCaption("Progress: 0%");

		// Set queue buttons for each queued unit ( up to 5 )
		auto unitDefIt = trainQueue->GetQueuedUnits().begin();
		unsigned int maxUnits = std::min(trainQueue->GetQueueSize(), 5u);
		for(unsigned int i = 0; i < maxUnits; ++i)
		{
			_queueButtons[i]->setVisible(true);
			_queueButtons[i]->setImageResource( unitDefIt->Value->GetIcon() );
			++unitDefIt;
		}

		// If there's more than 5 units in queue ( for now 5 is max )
		// some sign like (++) is shown
	}

	void UnitTrainingSubPanel::OnUnitTrained(Unit* owner, Unit* trainedUnit)
	{
		// TrainingQueue is not yet updated -> move all butons 1 cell left
		auto trainQueue = _currentUnit->GetTrainingQueue();
		if(trainQueue->GetQueueSize() == 0)
		{
			// No more queued units -> only hide currentUnit button
			_currentUnitButton->setVisible(false);
			return;
		}
		else
		{
			// Set first queued unit for currentUnitButton
			auto unitDefIt = trainQueue->GetQueuedUnits().begin();
			_currentUnitButton->setVisible(true);
			//_currentUnitButton->setImageResource( unitDefIt->Value->GetIcon() );
			_nameText->setCaption("Training: " + unitDefIt->Value->GetGuiName());
			_progressText->setCaption("Progress: 0%");

			// Set rest of queued units buttons
			++unitDefIt;
			unsigned int maxUnits = std::min(trainQueue->GetQueueSize()-1, 5u);
			for(unsigned int i = 0; i < maxUnits; ++i)
			{
				//_queueButtons[i]->setImageResource( unitDefIt->Value->GetIcon() );
				++unitDefIt;
			}

			if(maxUnits < 5u)
			{
				// If there's less than 5 units in queue, hide last one
				_queueButtons[maxUnits]->setVisible(false);
			}
		}
	}

	void UnitTrainingSubPanel::OnUnitRemovedFromQueue(Unit* owner, UnitDefinition* removedUnit, uint32 pos)
	{
		SetTrainingButtons();
	}

	void UnitTrainingSubPanel::OnUnitQueued(Unit* owner, UnitDefinition* queuedUnit)
	{
		SetTrainingButtons();
	}

	void UnitTrainingSubPanel::Resize(float ratioHorizontal, float ratioVertical)
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

		const MyGUI::IntCoord& buttonCoord = _currentUnitButton->getCoord();
		_currentUnitButton->setCoord(
			(int)((float)buttonCoord.left * ratioHorizontal), (int)((float)buttonCoord.top * ratioVertical), 
			(int)((float)buttonCoord.width * ratioHorizontal), (int)((float)buttonCoord.height * ratioVertical));

		for(unsigned int i = 0; i < _queueButtons.size(); ++i)
		{
			const MyGUI::IntCoord& queueCoord  = _queueButtons[i]->getCoord();
			_queueButtons[i]->setCoord(
				(int)((float)queueCoord.left * ratioHorizontal), (int)((float)queueCoord.top * ratioVertical), 
				(int)((float)queueCoord.width * ratioHorizontal), (int)((float)queueCoord.height * ratioVertical));
		}
	}
}