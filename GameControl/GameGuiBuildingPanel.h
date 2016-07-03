#pragma once

#include "GameGuiIFunctionalPanel.h"
#include "GameGuiConstructionButton.h"
#include <Array.h>
#include <Event.h>
#include <GUI.h>

namespace MyGUI
{
	class ImageBox;
	class Button;
}

namespace FlagRTS
{
	class UnitGroup;
	class UnitDefinition;
	
	namespace GameGui
	{
		class BuildingPanel  : public IFunctionalPanel
		{
		private:
			Array<TechBuilding*> _buildings;
			Array<ConstructionButton*> _buttons;
			Event<TechBuilding*> _buttonClicked;
			MyGUI::Button* _nextPageEconomyButton;
			MyGUI::Button* _prevoiusEconomyPageButton;
			MyGUI::Button* _nextPageMilitaryButton;
			MyGUI::Button* _prevoiusMilitaryPageButton;
			int _firstEcoBuilding;
			int _firstMilitBuilding;

			int _buttonsCols;
			int _buttonsRows;
			int _offX;
			int _offY;
			int _bsizeX;
			int _bsizeY;
			int _bmarginX;
			int _bmarginY;

		public:
			BuildingPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~BuildingPanel();

			void Show();
			void Hide();

			// Updates size of panel
			// Must be called if parent's size is changed
			void Resize(float ratioHorizontal, float ratioVertical);

			MyGUI::ImageBox* GetPanel() const { return _mainPanel; }
			ConstructionButton* GetButton(const unsigned int x, const unsigned int y) const { return _buttons[x + y*_buttonsCols]; }
	
			Event<TechBuilding*>& ButtonClicked() { return _buttonClicked; }

			void UpdateBuildingDefinitions(const Array<TechBuilding*>& newBuildings);

		private:
			void OnButtonPressed(MyGUI::Widget* button);
			void OnNextEcoPageClicked(MyGUI::Widget* button);
			void OnPrevEcoPageClicked(MyGUI::Widget* button);
			void OnNextMilitPageClicked(MyGUI::Widget* button);
			void OnPrevMilitPageClicked(MyGUI::Widget* button);
		};
	}
}