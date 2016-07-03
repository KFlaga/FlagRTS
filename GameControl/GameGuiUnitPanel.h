#pragma once

#include "GameGuiIFunctionalPanel.h"
#include "GameGuiIUnitSubPanel.h"
#include <GUI.h>
#include <Event.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
}

namespace FlagRTS
{
	class GUI;
	class Layout;
	class Unit;
	class SceneObject;

	namespace GameGui
	{
		class ModelViewer;

		class UnitPanel : public IFunctionalPanel
		{
		private:
			Array<IUnitSubPanel*> _subPanels;
			IUnitSubPanel* _currentSubPanel;
			Unit* _currentUnit;
			ModelViewer* _model;

		public:
			UnitPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~UnitPanel();

			// Updates current info on unit, as it could change
			void Update(float ms);

			void Show();
			void Hide();

			// Changes unit whose info is currently dispalyed ->
			// only changes unit in this panel, not subpanel - to update subpanel
			// ChangeSubPanel() must be called with same panel type
			void SetCurrentUnit(Unit* unit);

			template<typename PanelType = IUnitSubPanel>
			PanelType* GetSubPanel(int type) 
			{ 
				return static_cast<PanelType*>(_subPanels[type]); 
			}

			IUnitSubPanel* GetCurrentSubPanel() { return _currentSubPanel; }
			void ChangeSubPanel(int type);

			void Resize(float ratioHorizontal, float ratioVertical);

		private:
			void OnUnitDespawned(SceneObject*);
			DEFINE_DELEGATE1(UnitDespawnedDelegate,
				UnitPanel, OnUnitDespawned, SceneObject*);
			UnitDespawnedDelegate _onUnitDespawned;
		};
	}
}