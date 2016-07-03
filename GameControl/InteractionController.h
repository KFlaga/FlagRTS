#pragma once

#include "PhysicalObject.h"
#include <Event.h>
#include <ArrayMap.h>
#include <map>

namespace Ogre
{
	class Ray;
	class SceneManager;
}

namespace FlagRTS
{
	class SelectionBox;
	class InteractionController;
	class HoverController;
	class SelectionController;
	class UnitSelector;
	class GuiController;
	class CameraController;
	class CommandTarget;
	class Command;
	class CommandButton;
	class UnitGroup;
	class Unit;
	class UnitDefinition;
	class TechnologyController;
	struct TechBuilding;

	class InputControllerState
	{
	public:
		enum ICStateType : int
		{
			Off, // Mouse input handled in other place
			SelectionIdle, // Selection type context, no input action taken yet 
			SelectionSelecting, // Selection type context, selection box shown
			NoSelectionIdle, // NoSelection type context, no input action taken yet 
			NoSelectionSelecting, // NoSelection type context, selection box shown
			CommandTarget, // CommandTarget type context
			CommandArea, // CommandTarget type context
			ConstructionPlan, // Construction plan requested, abort on right, try place on left ( hotkeys may change state )
		};

	protected:
		InteractionController* _inputController;
		ICStateType _type;

	public:
		InputControllerState(ICStateType type, InteractionController* ic) : 
			_inputController(ic),
			_type(type)
		{ }

		virtual ~InputControllerState() { }
		virtual void Update() = 0;

		ICStateType GetStateType() const { return _type; }
	};

	class InteractionController
	{
		typedef InputControllerState::ICStateType ICStateType;
	private:
		HoverController* _hoverCtrl;
		SelectionController* _selectionCtrl;
		GuiController* _guiCtrl;
		TechnologyController* _techCtrl;
		CameraController* _camCtrl;
		ArrayMap<ICStateType, InputControllerState*> _states;
		InputControllerState* _curState;
		InputControllerState* _prevState;
		CommandTarget* _target;
		ArrayMap<CommandButton*,Command*> _contextCommands; // Commands active in current context
		Command* _choosenCommand;

		bool _wasMinimapLeftClicked;
		bool _wasMinimapRightClicked;
		Vector3 _minimapClickPos;
		Unit* _minimapClickUnit;

	public:
		InteractionController(Ogre::SceneManager* , SelectionController* , 
			GuiController*, TechnologyController*, CameraController*);
		~InteractionController();
		void Update(const Ogre::Ray& cameraRay);

		void BeginInputProcessing();

		void ChangeState(ICStateType state)
		{
			_prevState = _curState;
			_curState = _states[state];
		}

		void PreviousState()
		{
			std::swap(_curState, _prevState);
		}

		void StartSelection();
		void UpdateSelection();
		void EndSelection();
		void AbortSelection();
		void SelectHoveredUnit();
		
		// - Check registered commands requirements and update them if availablity changed
		// - Check if any of registered hotkeys was pressed
		//	Returns true if any was
		bool UpdateContextCommands();
		
		CommandTarget* GetCommandTarget() { return _target; }
		HoverController* GetHoverController() { return _hoverCtrl; }
		SelectionController* GetSelectionController() { return _selectionCtrl; }
		GuiController* GetGuiController() { return _guiCtrl; }
		Command* GetChoosenCommand() { return _choosenCommand; }

		// Adds hotkey to be listened to and command to be choosen when hotkey is pressed
		// + registers command button to gui
		void RegisterContextCommand(CommandButton* button, Command* command);
		// Removes all hotkeys and command buttons
		void ResetContextCommands();
		// Aborts choosen command and returns back to idle state
		void AbortCommand();
		// Orders choosen command and returns back to idle state
		void OrderChoosenCommand();
		// Starts construction plan, changes ICState to ConstructionPlan
		void StartConstructionPlan(TechBuilding* building);
		// Aborts construction plan, changes ICState to Idle
		void AbortConstructionPlan();
		// Accepts construction plan if position is correct, otherwise
		// notifies user that position is bad ( or not? ) and continue planning
		// If shift is pressed and plan succeded starts same plan again
		void AcceptConstructionPlan();
		// Sets new plan position
		void UpdateConstructionPlanPosition(const Vector3& pos);

		bool WasMinimapLeftClicked() const { return _wasMinimapLeftClicked; }
		bool WasMinimapRightClicked() const { return _wasMinimapRightClicked; }
		const Vector3& GetMinimapClickPosition() const { return _minimapClickPos; }
		Unit* GetMinimapUnitClicked() const { return _minimapClickUnit; }

	private:
		// Changes state to proper selection-type one 
		void ChangeStateToSelection();
		// Changes state to proper idle-type one 
		void ChangeStateToIdle();

		// Called when command button/hotkey is choosen
		// Changes context/state to one proper for command
		void CommandChoosen(Command* command);
		
		void OnCommandChoosen(CommandButton* button);
		DEFINE_DELEGATE1(CommandChooenDelegate, InteractionController,
			OnCommandChoosen, CommandButton*);
		CommandChooenDelegate _onCommandChoosen;

		void InstantCommandChoosen();
		void TargetCommandChoosen();
		
		void CheckContextCommandsReqs();

		void OnSelectionChanged(UnitGroup* selectedGroup);
		DEFINE_DELEGATE1(SelectionChangedDelegate, InteractionController,
			OnSelectionChanged, UnitGroup*);
		SelectionChangedDelegate _onSelectionChanged;
		
		void OnContextSubgroupChanged(Array<Unit*>* subgroup);
		DEFINE_DELEGATE1(SubgroupChangedDelegate, InteractionController,
			OnContextSubgroupChanged, Array<Unit*>*);
		SubgroupChangedDelegate _onSubgroupChanged;

		void OnUnitButtonClicked(Unit* unitChoosen, bool fromContextSubgroup);
		DEFINE_DELEGATE2(UnitButtonClickedDelegate, InteractionController,
			OnUnitButtonClicked, Unit*, bool);
		UnitButtonClickedDelegate _onUnitButtonClicked;

		void OnBuildingAvailabilityChanged(TechBuilding* buidling, uint8 player);
		DEFINE_DELEGATE2(BuildingAvailabilityChangedDelegate, InteractionController,
			OnBuildingAvailabilityChanged,TechBuilding*, uint8);
		BuildingAvailabilityChangedDelegate _onBuildingAvailabilityChanged;

		void OnBuildingButtonClicked(TechBuilding* selectedBuilding);
		DEFINE_DELEGATE1(BuildingButtonClickedDelegate, InteractionController,
			OnBuildingButtonClicked, TechBuilding*);
		BuildingButtonClickedDelegate _onBuildingButtonClicked;

		void OnMinimapLeftDragged(Vector2 minimapPos);
		DEFINE_DELEGATE1(MinimapLeftDraggedDelegate, InteractionController,
			OnMinimapLeftDragged, Vector2);
		MinimapLeftDraggedDelegate _onMinimapLeftDragged;

		void OnMinimapLeftClicked(Vector2 minimapPos);
		DEFINE_DELEGATE1(MinimapLeftClickedDelegate, InteractionController,
			OnMinimapLeftClicked, Vector2);
		MinimapLeftClickedDelegate _onMinimapLeftClicked;

		void OnMinimapRightDragged(Vector2 minimapPos);
		DEFINE_DELEGATE1(MinimapRightDraggedDelegate, InteractionController,
			OnMinimapRightDragged, Vector2);
		MinimapRightDraggedDelegate _onMinimapRightDragged;

		void OnMinimapRightClicked(Vector2 minimapPos);
		DEFINE_DELEGATE1(MinimapRightClickedDelegate, InteractionController,
			OnMinimapRightClicked, Vector2);
		MinimapRightClickedDelegate _onMinimapRightClicked;
	};

	class ICStateOff : public InputControllerState
	{
	public:
		ICStateOff(InteractionController* ic);
		void Update() { }
	};

	class ICStateSelectionIdle : public InputControllerState
	{
	public:
		ICStateSelectionIdle(InteractionController* ic);
		void Update();
	};

	class ICStateSelectionSelecting : public InputControllerState
	{
		bool _mouseMoved;
	public:
		ICStateSelectionSelecting(InteractionController* ic);
		void Update();
	};
	
	class ICStateNoSelectionSelecting : public InputControllerState
	{
		bool _mouseMoved;
	public:
		ICStateNoSelectionSelecting(InteractionController* ic);
		void Update();
	};
	
	class ICStateNoSelectionIdle : public InputControllerState
	{
	public:
		ICStateNoSelectionIdle(InteractionController* ic);
		void Update();
	};

	class ICStateConstructionPlan : public InputControllerState
	{
	public:
		ICStateConstructionPlan(InteractionController* ic);
		void Update();
	};

	class ICStateCommandTarget : public InputControllerState
	{
	public:
		ICStateCommandTarget(InteractionController* ic);
		void Update();
	};

	class ICStateCommandArea : public InputControllerState
	{
	public:
		ICStateCommandArea(InteractionController* ic);
		void Update();
	};
}