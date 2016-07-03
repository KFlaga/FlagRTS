#pragma once

#include "GameGuiIFunctionalPanel.h"
#include "GameGuiUnitButton.h"
#include <UnitGroup.h>
#include <GUI.h>

namespace MyGUI
{
	class Button;
}

namespace FlagRTS
{
	class UnitGroup;
	
	namespace GameGui
	{
		// Contains UnitButtons that represent selected unit group
		// If one is clicked, then unit subgroup changes to selected unit's one
		// If unit button from active subgroup is clicked, only this unit
		// is selected
		// Click + shift deselects unit
		// Click + controls lefts only this subgroup in selection
		class SelectionPanel  : public IFunctionalPanel
		{
		private:
			Array<UnitButton*> _buttons;
			MyGUI::Button* _nextPageButton;
			MyGUI::Button* _prevoiusPageButton;

			int _buttonsCols;
			int _buttonsRows;
			int _offX;
			int _offY;
			int _frameX;
			int _frameY;
			int _bsizeX;
			int _bsizeY;
			int _bmarginX;
			int _bmarginY;
			int _activeButtonPage;

			UnitGroup* _selectedUnits;
			UnitGroup::UnitMap::iterator _lastUnitInPage;
			size_t _contextSubgroupType;

			Event<Unit*, bool> _buttonClicked;

		public:
			SelectionPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~SelectionPanel();

			void Show();
			void Hide();

			// Updates size of panel
			// Must be called if parent's size is changed
			// Values are of relative size of parent
			void Resize(float ratioHorizontal, float ratioVertical);

			void SetSelectedUnitGroup(UnitGroup* group);
			void SetContextSubgroup(Array<Unit*>* subgroup);

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }
			UnitButton* GetButton(const unsigned int x, const unsigned int y) const { return _buttons[x + y*_buttonsCols]; }

			// If there's more than 16 units in selection, shows
			// next 16 units 
			void NextButtonPage();
			
			// If there's more than 16 units in selection, shows
			// prevoious 16 units 
			void PreviousButtonPage();

			// Event that occurs when one of unit button is clicked
			// First argument is unit that button represents, second is 
			// true if unit is of context subgroup type
			Event<Unit*, bool>& ButtonClicked() { return _buttonClicked; }

		private:
			void BindUnitsToButtons();
			void OnButtonPressed(MyGUI::Widget* button);
		};
	}
}