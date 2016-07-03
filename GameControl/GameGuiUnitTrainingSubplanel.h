#pragma once

#include "GameGuiIUnitSubPanel.h"
#include <GUI.h>
#include <Event.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class TextBox;
	class Button;
}

namespace FlagRTS
{
	class Unit;
	class UnitDefinition;

	namespace GameGui
	{
		class UnitTrainingSubPanel : public IUnitSubPanel
		{
		private:
			MyGUI::TextBox* _nameText;
			MyGUI::TextBox* _progressText;
			MyGUI::Button* _currentUnitButton;
			Array<MyGUI::Button*> _queueButtons;
			bool _trainAbortPending;
			bool _removeFromQueuePending;
			unsigned int _removePosition;

		public:
			UnitTrainingSubPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout, UnitPanel* parent);
			~UnitTrainingSubPanel();

			// Updates current info on unit, as it could change
			void Update(float ms);

			void Show();
			void Hide();

			// Changes unit whose training queue is currently dispalyed
			void SetCurrentUnit(Unit* unit);

			void Resize(float ratioHorizontal, float ratioVertical);
		
		private:
			void OnCurrentUnitButtonClicked(MyGUI::Widget* button);
			void OnQueuedUnitButtonClicked(MyGUI::Widget* button);
			void SetTrainingButtons();
		 
			void OnUnitTrained(Unit* owner, Unit* trainedUnit);
			DEFINE_DELEGATE2(UnitTrainedDelegate, UnitTrainingSubPanel,
				OnUnitTrained, Unit*, Unit*);
			UnitTrainedDelegate _onUnitTrained;

			void OnUnitRemovedFromQueue(Unit* owner, UnitDefinition* removedUnit, uint32 pos);
			DEFINE_DELEGATE3(UnitRemovedFromQueueDelegate, UnitTrainingSubPanel,
				OnUnitRemovedFromQueue, Unit*, UnitDefinition*, uint32);
			UnitRemovedFromQueueDelegate _onUnitRemovedFromQueue;

			void OnUnitQueued(Unit* owner, UnitDefinition* queuedUnit);
			DEFINE_DELEGATE2(UnitQueuedDelegate, UnitTrainingSubPanel,
				OnUnitQueued, Unit*, UnitDefinition*);
			UnitQueuedDelegate _onUnitQueued;
		};
	}
}